﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimSkillBase.h"

#include "../Animation/R4AnimationInterface.h"
#include "../Util/UtilAnimation.h"

#include <Net/UnrealNetwork.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimSkillBase)

UR4AnimSkillBase::UR4AnimSkillBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault( true );
}

#if WITH_EDITOR
void UR4AnimSkillBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if ( PropertyChangedEvent.MemberProperty == nullptr )
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Section을 읽어와 배열을 자동으로 채움. 하하 아주 편리하지?
	if ( FStructProperty* prop = CastField<FStructProperty>( PropertyChangedEvent.MemberProperty );
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct() )
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>( this );
		UAnimMontage* anim = animInfo->SkillAnim;

		if ( !IsValid( anim ) )
			return;

		// Section 찾기
		TSet<FName> nowSections;
		for ( int32 idx = 0; idx < anim->GetNumSections(); idx++ )
		{
			FName sectionName = anim->GetSectionName( idx );
			nowSections.Emplace( sectionName );
		}

		// 필요 없는 Section 제거
		for ( auto it = animInfo->DetectExecutes.CreateIterator(); it; ++it )
		{
			if( !nowSections.Contains( it->Key ) )
				it.RemoveCurrent();
		}

		for ( auto it = animInfo->BuffExecutes.CreateIterator(); it; ++it )
		{
			if( !nowSections.Contains( it->Key ) )
				it.RemoveCurrent();
		}

		// 필요한 Section 추가
		for( const auto& sectionName : nowSections )
		{
			if( !animInfo->DetectExecutes.Contains( sectionName ) )
				animInfo->DetectExecutes.Add( sectionName );

			if( !animInfo->BuffExecutes.Contains( sectionName ) )
				animInfo->BuffExecutes.Add( sectionName );
		}
	}
}
#endif

void UR4AnimSkillBase::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( UR4AnimSkillBase, AnimPlayServerStates );
}

void UR4AnimSkillBase::BeginPlay()
{
	Super::BeginPlay();

	_ParseSkillAnimInfo();
	
	// 일단 Tick Off 상태로 시작
	SetComponentTickEnabled( false );
}

/**
 *  Tick
 */
void UR4AnimSkillBase::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	float nowServerTime = R4GetServerTimeSeconds( GetWorld() );
	if ( nowServerTime < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("nowServerTime is invalid.") );
		return;
	}
	
	_UpdateExecute( nowServerTime );

	// Update가 더 이상 필요 없다면 Tick Off
	if ( !IsNeedTick() )
		SetComponentTickEnabled( false );
}

void UR4AnimSkillBase::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	if( GetOwnerRole() == ROLE_Authority )
		AnimPlayServerStates.Empty();
	
	PendingExecutes.Empty();
	CachedSkillAnimInfos.Empty();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *  Skill Anim Key에 맞는, 특정 시간 뒤에 Execute 예약 추가.
 *  멤버로 등록된 Skill Anim Key만 추가 가능.
 *  InFunc에 Update 배열에 무언가를 추가하는 로직 작성 시 추가 로직 반복에 의한 메모리 누수 주의.
 *  @param InSkillAnimKey : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InFunc : 실행시킬 Lambda, R-value, this 캡쳐 시 weak 캡쳐할 것
 *  @param InDelay : 실행 Delay. ( Delay <= 0 이면, 바로 실행 됨 ) 
 *  @param InDelayRate : Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddExecute( int32 InSkillAnimKey, TFunction<void()>&& InFunc, float InDelay, float InDelayRate )
{
	if ( !IsValidSkillAnimKey(InSkillAnimKey) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return;
	}

	if( !InFunc )
	{
		LOG_WARN( R4Skill, TEXT("InFunc is invalid.") );
		return;
	}

	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// division by zero 방지
	InDelayRate = FMath::Max( KINDA_SMALL_NUMBER, InDelayRate );
	
	// 실제로 적용할 delay 계산
	InDelay /= InDelayRate;

	// Delay가 너무 작으면, 그냥 바로 실행
	if ( InDelay <= KINDA_SMALL_NUMBER )
	{
		if( InFunc )
			InFunc();

		return;
	}
	
	// 아니면 Update 대기열에 넣고 Tick On
	PendingExecutes.Emplace
	(
		FR4AnimSkillExecuteInfo
		(
			InSkillAnimKey,
			_GetNowMontageInstanceID( InSkillAnimKey ),
			R4GetServerTimeSeconds( GetWorld() ) + InDelay,
			MoveTemp(InFunc)
		)
	);
	
	SetComponentTickEnabled( true );
}

/**
 *  Detect Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeDetectInfo : Delay와 Detect 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddDetectExecute( int32 InSkillAnimKey, const FR4SkillTimeDetectWrapper& InTimeDetectInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Replicated Detector일 시 Server에서만 Spawn 하는지 확인
	{
		// Parse Net Flag
		bool bReplicate = InTimeDetectInfo.DetectEffect.DetectorInfo.DetectClass.GetDefaultObject()->GetIsReplicated();
		bool bServerSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) );
		bool bLocalSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Local ) );
		bool bSimulatedSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) );
		
		if ( !ensureMsgf( bReplicate ? ( bServerSpawn && !bLocalSpawn && !bSimulatedSpawn ) : true,
				TEXT("Replicated Detector must be spawned only on Server") ) )
			return;
	}
	
	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &detectInfo = InTimeDetectInfo.DetectEffect]()
	{
		if ( thisPtr.IsValid() )
			thisPtr->ExecuteDetect( detectInfo );
	}, InTimeDetectInfo.DelayTime, InDelayRate );
}

/**
 *  buff Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeBuffInfo : buff 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddBuffExecute( int32 InSkillAnimKey, const FR4SkillTimeBuffWrapper& InTimeBuffInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeBuffInfo.SkillBuffInfo.BuffNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &InTimeBuffInfo]()
	{
		if ( thisPtr.IsValid() )
		{
			// 나에게 버프 적용
			thisPtr->ApplySkillBuff( InTimeBuffInfo.SkillBuffInfo );
		}
	}, InTimeBuffInfo.DelayTime, InDelayRate );
}

/**
 *  Section의 Length와, 시작 시간이 주어졌을 때, 현재 서버타임과 비교하여 얼마나 빠르게 실행해야 서버와 동일하게 끝낼 수 있는지 delay 계산
 *  @param InTotalLength : 실행 시간 ( ex ) Anim Section의 Length
 *  @param InStartTime : 현재 ServerTime과 비교할 시작 시간
 *  @return : TotalLength < Delay일 시, -1.f 반환
 */
float UR4AnimSkillBase::CalculateDelayRate( float InTotalLength, float InStartTime )
{
	float delay = FMath::Max( KINDA_SMALL_NUMBER, ( R4GetServerTimeSeconds( GetWorld() ) - InStartTime ) );

	// Total Length가 음수일 경우, Loop라고 판단, 1로 실행.
	if( InTotalLength < 0 )
		return 1.f;
	
	// delay가 InTotalLength보다 길면 -1.f return
	if( delay > InTotalLength )
		return -1.f;
	
	// DelayRate = 전체시간 / 남은시간
	return ( InTotalLength / FMath::Max( ( InTotalLength - delay ), KINDA_SMALL_NUMBER ) );
}

/**
 *  Skill Animation을 Local에서 Play. InStartServerTime으로 동기화 가능.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InStartSectionName : 시작 된 Section Name.
 *  @param InStartServerTime : Skill Anim이 시작 된 Server Time.
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnimSync_Local( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return false;
	}

	if ( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ), TEXT("Skill Anim is nullptr.") ) )
		return false;
	
	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>( GetOwner() );
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return false;
	}

	// 같은 Anim을 두번 사용하면, Blend Out 시점을 End Play Anim으로 판정하더라도
	// Begin 2 () -> End 1 () 순서로 로직이 진행되어 문제가 생김. ( Montage Instance는 파괴되지만, Anim은 queue에서 대기 )
	// 그래서 Montage Instance 파괴 시점을 Get
	FOnClearMontageInstance* montageInstanceDelegate = owner->OnClearMontageInstance();
	if( montageInstanceDelegate == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("montageInstanceDelegate is nullptr.") )
		return false;
	}
	
	// Start Section Name이 invalid하면, 시작 Section으로 설정
	FName startSectionName = InStartSectionName;
	if( !InSkillAnimInfo.SkillAnim->IsValidSectionName( startSectionName ) )
		startSectionName = InSkillAnimInfo.SkillAnim->GetSectionName( 0 );
		
	// Local에서 Play
	owner->PlayAnimSync( InSkillAnimInfo.SkillAnim, startSectionName, 1.f, InStartServerTime );
	FAnimMontageInstance* montageInstance = owner->GetActiveInstanceForMontage( InSkillAnimInfo.SkillAnim );
	if ( montageInstance == nullptr )
	{
		LOG_ERROR( R4Skill, TEXT("FAnimMontageInstance is nullptr.") )
		return false;
	}

	// Anim Play End시 로직 설정, 
	// this capture, use weak lambda
	FDelegateHandle handle = montageInstanceDelegate->AddWeakLambda( this,
		[this, &InSkillAnimInfo, mID = montageInstance->GetInstanceID()]
		( FAnimMontageInstance& InStoppedMontageInstance )
			{
				if( InStoppedMontageInstance.GetInstanceID() != mID )
					return;
				
				IR4AnimationInterface* ownerInterface = Cast<IR4AnimationInterface>( GetOwner() );
				if ( ownerInterface == nullptr )
					return;
			
				FOnClearMontageInstance* mIDelegate = ownerInterface->OnClearMontageInstance();
				if( mIDelegate == nullptr )
					return;
	
				// END
				OnEndSkillAnim( InSkillAnimInfo );
			
				// UnBind Delegates..
				FDelegateHandle dHandle;
				CachedClearMontageHandles.RemoveAndCopyValue( mID, dHandle );
				mIDelegate->Remove( dHandle );
			} );

	// Add DelegateHandle
	CachedClearMontageHandles.Emplace( montageInstance->GetInstanceID(), MoveTemp( handle ) );
	OnBeginSkillAnim( InSkillAnimInfo, startSectionName, R4GetServerTimeSeconds( GetWorld() ) );

	return true;
}

/**
 *  Skill Animation을 Play 후 Server RPC 전송을 통해 동기화.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InStartSectionName : 시작 된 Section Name.
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnim_WithServerRPC( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return false;
	}

	if ( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ), TEXT("Skill Anim is nullptr.") ) )
		return false;

	// 현재 서버 시간.
	float nowServerTime = R4GetServerTimeSeconds( GetWorld() );

	// Start Section Name이 invalid하면, 시작 Section으로 설정
	FName startSectionName = InStartSectionName;
	if( !InSkillAnimInfo.SkillAnim->IsValidSectionName( startSectionName ) )
		startSectionName = InSkillAnimInfo.SkillAnim->GetSectionName( 0 );
	
	// ROLE_AutonomousProxy인 경우 로컬에서 먼저 플레이 후 Server RPC 전송
	if( GetOwnerRole() == ROLE_AutonomousProxy )
	{
		bool bPlayAnim = PlaySkillAnimSync_Local( InSkillAnimInfo, startSectionName, nowServerTime );

		if ( !bPlayAnim )
			return false;
	}

	// Server로 Play 요청
	int32 startIndex = InSkillAnimInfo.SkillAnim->GetSectionIndex( startSectionName );
	_ServerRPC_PlaySkillAnim( InSkillAnimInfo.SkillAnimServerKey, startIndex, nowServerTime );
	return true;
}

/**
 * Anim을 Play시작 시 호출.
 * @param InSkillAnimInfo : Play된 Skill Anim 정보
 * @param InStartSectionName : 시작 된 Section Name.
 * @param InStartServerTime : Skill Anim이 시작된 Server Time
 */
void UR4AnimSkillBase::OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return;
	}

	if ( !IsValid( InSkillAnimInfo.SkillAnim ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim is Invalid. Check SkillAnim Replicate State.") );
		return;
	}
	
	int32 sectionIndex = InSkillAnimInfo.SkillAnim->GetSectionIndex( InStartSectionName );
	sectionIndex = ( sectionIndex == INDEX_NONE ) ? 0 : sectionIndex;
	
	if( GetOwnerRole() == ROLE_Authority )
	{
		// Server Skill Anim 사용 상태 설정
		AnimPlayServerStates[InSkillAnimInfo.SkillAnimServerKey].SectionIndex = sectionIndex; 
		AnimPlayServerStates[InSkillAnimInfo.SkillAnimServerKey].AnimStartServerTime = InStartServerTime;
		AnimPlayServerStates[InSkillAnimInfo.SkillAnimServerKey].MontageInstanceID = _GetNowMontageInstanceID( InSkillAnimInfo.SkillAnimServerKey );
	}

	// Add executes
	// Execute Delay Rate는 Anim과 동기화
	float sectionLength = UtilAnimation::GetCompositeAnimLength( InSkillAnimInfo.SkillAnim, sectionIndex );
	float executeDelayRate = CalculateDelayRate( sectionLength, InStartServerTime );
	
	// delay가 너무 길어졌으면 Skip
	if( executeDelayRate < 0.f )
	{
		LOG_N( R4Skill, TEXT("Execute delay is too late. Anim Section Length : [%f], delay : [%f]"), sectionLength, R4GetServerTimeSeconds( GetWorld() ) - InStartServerTime );
		return;
	}

	// Add Execute Detect
	if ( auto it = InSkillAnimInfo.DetectExecutes.Find( InStartSectionName ) )
	{
		for ( const auto& detect : it->DetectEffects )
			AddDetectExecute( InSkillAnimInfo.SkillAnimServerKey, detect, executeDelayRate );
	}

	// Add Execute Buffs
	if ( auto it = InSkillAnimInfo.BuffExecutes.Find( InStartSectionName ) )
	{
		for ( const auto& buff : it->Buffs )
			AddBuffExecute( InSkillAnimInfo.SkillAnimServerKey, buff, executeDelayRate );
	}
}

/**
 *  Anim 종료 시 호출.
 * @param InSkillAnimInfo : End된 Skill Anim 정보
 */
void UR4AnimSkillBase::OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return;
	}

	if ( !IsValid( InSkillAnimInfo.SkillAnim ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim is Invalid. Check SkillAnim Replicate State.") );
		return;
	}
	
	if( GetOwnerRole() == ROLE_Authority )
	{
		// Skill Anim 사용 상태 해제
		AnimPlayServerStates[InSkillAnimInfo.SkillAnimServerKey].AnimStartServerTime = -1.f;
	}
}

/**
 *  Server RPC의 Validation check 시 사용.
 *  기본적으로 유효한 Anim Server Key로 요청인지 확인
 */
bool UR4AnimSkillBase::PlaySkillAnim_Validate( int32 InSkillAnimKey ) const
{
	// 유효한 Server Key로 요청 시 Validate 성공
	return IsValidSkillAnimKey( InSkillAnimKey );
}

/**
 *  Server RPC의 Play Skill Anim 시 요청 무시 check에 사용.
 *  기본적으로 Skill을 사용할 수 있는 상태인지 확인
 */
bool UR4AnimSkillBase::PlaySkillAnim_Ignore( int32 InSkillAnimKey ) const
{
	return !CanActivateSkill();
}

/**
 *  Key에 맞는 Skill Anim이 Server에서 Play되고 있는지 확인 
 */
bool UR4AnimSkillBase::IsSkillAnimServerPlaying( int32 InSkillAnimKey ) const
{
	if( !IsValidSkillAnimKey( InSkillAnimKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return false;
	}

	return AnimPlayServerStates[InSkillAnimKey].AnimStartServerTime > - KINDA_SMALL_NUMBER;
}

/**
 *  Key에 맞는 멤버로 등록된 FR4SkillAnimInfo를 찾아서 return
 *  @return : 찾을 수 없는 경우 nullptr
 */
const FR4SkillAnimInfo* UR4AnimSkillBase::GetSkillAnimInfo( int32 InSkillAnimKey ) const
{
	if ( !IsValidSkillAnimKey( InSkillAnimKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return nullptr;
	}

	// CachedSkillAnimInfos를 순회하여 Find
	for ( auto it = CachedSkillAnimInfos.CreateConstIterator(); it; ++it )
	{
		if ( ( *it )->SkillAnimServerKey == InSkillAnimKey )
			return *it;
	}

	return nullptr;
}

/**
 *  현재 Tick이 필요한 상태인지 return.
 *  기본적으로 tick은 off 상태를 유지하며, 특정 함수에서 tick을 on 시키면
 *  해당 함수 false 시 Tick 함수에서 다시 Off.
 */
bool UR4AnimSkillBase::IsNeedTick() const
{
	return PendingExecutes.Num() > 0;
}

/**
 *  실행 대기중인 Executes를 Update.
 *  @param InNowServerTime : 현재 서버 시간
 */
void UR4AnimSkillBase::_UpdateExecute( float InNowServerTime )
{
	for( auto it = PendingExecutes.CreateIterator(); it; ++it )
	{
		// 현재 Skill Anim montage의 Active Montage Instance ID와 일치하지 않으면, 제거
		if ( it->MontageInstanceID != _GetNowMontageInstanceID( it->SkillAnimKey ) )
		{
			it.RemoveCurrentSwap();
			continue;
		}
		
		// Update 가능 상태이고, 시간이 지났다면, execute 후 제거
		if ( InNowServerTime > it->ExecuteServerTime )
		{
			if ( it->Func )
				it->Func();
			
			it.RemoveCurrentSwap();
		}
	}
}

/**
 *  InSkillAnimKey의 Montage를 현재 Play 시키고 있는 Montage Instance ID를 Get
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 */
int32 UR4AnimSkillBase::_GetNowMontageInstanceID( int32 InSkillAnimKey ) const
{
	if ( !IsValidSkillAnimKey( InSkillAnimKey ) )
	{
		LOG_WARN( R4Skill, TEXT("InSkillAnimKey is invalid.") );
		return INDEX_NONE;
	}

	const FR4SkillAnimInfo* skillAnimInfo = GetSkillAnimInfo( InSkillAnimKey );
	if( skillAnimInfo == nullptr )
		return INDEX_NONE;
	
	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>( GetOwner() );
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return INDEX_NONE;
	}
	
	FAnimMontageInstance* montageInstance = owner->GetActiveInstanceForMontage( skillAnimInfo->SkillAnim );
	if ( montageInstance == nullptr )
		return INDEX_NONE;

	return montageInstance->GetInstanceID();
}

/**
 *  Server로 Animation Play를 요청.
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartSectionIndex : 시작한 Section Index
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
void UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Implementation( int32 InSkillAnimKey, int32 InStartSectionIndex, float InStartServerTime )
{
	// 특정 경우 요청을 무시.
	if ( PlaySkillAnim_Ignore( InSkillAnimKey ) )
		return;
	
	const FR4SkillAnimInfo* skillAnimInfo = GetSkillAnimInfo( InSkillAnimKey );
	if ( ( skillAnimInfo == nullptr )
		|| ( !ensureMsgf( IsValid( skillAnimInfo->SkillAnim ), TEXT("Skill Anim is nullptr.") ) ) )
		return;
	
	// Server Anim Play
	FName startSectionName = skillAnimInfo->SkillAnim->GetSectionName( InStartSectionIndex );
	PlaySkillAnimSync_Local( *skillAnimInfo, startSectionName, InStartServerTime );
}

/**
 *  Server Animation Play를 요청의 유효성을 PlaySkillAnim_Validate로 확인.
 *  PlaySkillAnim_Validate() 을 Override하여 키에 따라 로직 작성
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartSectionIndex : 시작한 Section Index
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
bool UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Validate( int32 InSkillAnimKey, int32 InStartSectionIndex, float InStartServerTime )
{
	return PlaySkillAnim_Validate( InSkillAnimKey );
}

/**
 *  Skill Anim 멤버를 파싱
 *  Server에서는 Key값 부여도 진행
 */
void UR4AnimSkillBase::_ParseSkillAnimInfo()
{
	for ( const auto& [prop, value] : TPropertyValueRange<FStructProperty>( GetClass(), this ) )
	{
		if ( prop->Struct != FR4SkillAnimInfo::StaticStruct() )
			continue;
		
		const FR4SkillAnimInfo* c_animInfoPtr = static_cast<const FR4SkillAnimInfo*>(value);
		if ( !IsValid(c_animInfoPtr->SkillAnim) )
			continue;

		// 키값 부여 & 캐싱
		if ( FR4SkillAnimInfo* animInfoPtr = const_cast<FR4SkillAnimInfo*>(c_animInfoPtr) )
		{
			// 서버일 시, Skill Anim Info에 키값을 부여 ( 0 ~ N )
			if ( GetOwnerRole() == ROLE_Authority )
				animInfoPtr->SkillAnimServerKey = CachedSkillAnimInfos.Num();
			
			CachedSkillAnimInfos.Emplace( c_animInfoPtr );
		}
	}

	// SkillAnimPlayState를 Skill Anim만큼 초기화
	if ( GetOwnerRole() == ROLE_Authority )
		AnimPlayServerStates.SetNum( CachedSkillAnimInfos.Num() );
}

/**
 *  Server에서 Play되는 Anim 상태가 변동 시 호출
 */
void UR4AnimSkillBase::_OnRep_AnimPlayServerState( const TArray<FAnimPlayServerStateInfo>& InPrevAnimPlayServerStates )
{
	// simulated proxy only.
	if( GetOwnerRole() != ROLE_SimulatedProxy
		|| InPrevAnimPlayServerStates.Num() != AnimPlayServerStates.Num() )
		return;
	
	// Simulated Proxy 일 시, Play에 맞춘 상황을 호출
	for ( int32 idx = 0; idx < AnimPlayServerStates.Num(); idx++ )
	{
		bool bPrevPlay = InPrevAnimPlayServerStates[idx].AnimStartServerTime > - KINDA_SMALL_NUMBER;
		bool bNowPlay = AnimPlayServerStates[idx].AnimStartServerTime > - KINDA_SMALL_NUMBER;
		bool bSectionChange = ( InPrevAnimPlayServerStates[idx].SectionIndex != AnimPlayServerStates[idx].SectionIndex );
		bool bMIDChange = ( InPrevAnimPlayServerStates[idx].MontageInstanceID != AnimPlayServerStates[idx].MontageInstanceID );
		
		const FR4SkillAnimInfo* animInfo = GetSkillAnimInfo( idx );
		if ( animInfo == nullptr || !IsValid( animInfo->SkillAnim ) )
		{
			LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), idx );
			continue;
		}

		FName newSectionName = animInfo->SkillAnim->GetSectionName( AnimPlayServerStates[idx].SectionIndex );
		// 새로 Play가 시작 된 경우 || Section만 Change된 경우 || override play 인 경우
		if ( ( !bPrevPlay && bNowPlay )
			|| ( bPrevPlay && bNowPlay && ( bSectionChange || bMIDChange ) ) )
		{
			PlaySkillAnimSync_Local( *animInfo, newSectionName, AnimPlayServerStates[idx].AnimStartServerTime );
		}
	}
}