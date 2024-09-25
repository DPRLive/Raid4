// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimSkillBase.h"

#include "../Animation/Notify/R4NotifyByIdInterface.h"
#include "../Animation/R4AnimationInterface.h"
#include "../Buff/R4BuffBase.h"
#include "../Buff/R4BuffReceiveInterface.h"

#include <Net/UnrealNetwork.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimSkillBase)

UR4AnimSkillBase::UR4AnimSkillBase()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault( true );

	Server_CachedSkillAnimInfoCount = 0;
}

#if WITH_EDITOR

void UR4AnimSkillBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Notify를 읽어와 배열을 자동으로 채움. 하하 아주 편리하지?
	if ( FStructProperty* prop = CastField<FStructProperty>( PropertyChangedEvent.MemberProperty );
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct() )
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>( this );
		UAnimMontage* anim = animInfo->SkillAnim;

		if ( !IsValid( anim ) )
			return;

		// Detect, ApplyBuff Notify의 index들을 찾아냄 
		TSet<int32> detectIdx, buffIdx;
		for ( int32 i = 0; i < anim->Notifies.Num(); i++ )
		{
			IR4NotifyByIdInterface* notify = Cast<IR4NotifyByIdInterface>( anim->Notifies[i].Notify );
			if ( notify == nullptr )
				continue;
			
			if ( notify->GetNotifyType() == ER4AnimNotifyType::Detect )
				detectIdx.Emplace( i );

			if ( notify->GetNotifyType() == ER4AnimNotifyType::ApplyBuff )
				buffIdx.Emplace( i );
		}

		/**
		*  편집 시 편하도록 Array의 원소들을 Index Set을 보고 동기화
 		*  Array에 없는 Set의 Idx = Array에 추가
 		*  Array와 Set에 있는 Set의 Idx = 무시
 		*  Array에 있는데 Set에 없는 Idx = Array에서 제거
 		*/
		
		// 필요 없는 index는 제거
		for ( auto it = animInfo->DetectNotifies.CreateIterator(); it; ++it )
		{
			// detectIdx에 없는 idx는 제거
			if ( detectIdx.Find( it->NotifyNumber ) == nullptr )
			{
				it.RemoveCurrentSwap();
				continue;
			}
			detectIdx.Remove( it->NotifyNumber );
		}

		for ( auto it = animInfo->BuffNotifies.CreateIterator(); it; ++it )
		{
			if ( buffIdx.Find( it->NotifyNumber ) == nullptr )
			{
				it.RemoveCurrentSwap();
				continue;
			}
			buffIdx.Remove( it->NotifyNumber );
		}
		
		// 기존에 없는 index는 추가
		for ( const auto& idx : detectIdx )
		{
			animInfo->DetectNotifies.Emplace( idx );
		}

		for ( const auto& idx : buffIdx )
		{
			animInfo->BuffNotifies.Emplace( idx );
		}
		
		// Detect notifies SORT
		animInfo->DetectNotifies.Sort( [](const FR4NotifyDetectWrapper& InElem1, const FR4NotifyDetectWrapper& InElem2 )
		{
			return InElem1.NotifyNumber < InElem2.NotifyNumber;
		} );

		// Buff Notifies SORT
		animInfo->BuffNotifies.Sort( [](const FR4NotifyBuffWrapper& InElem1, const FR4NotifyBuffWrapper& InElem2 )
		{
			return InElem1.NotifyNumber < InElem2.NotifyNumber;
		} );
	}
}
#endif

void UR4AnimSkillBase::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME_CONDITION( UR4AnimSkillBase, AnimPlayServerState, COND_OwnerOnly )
}

void UR4AnimSkillBase::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole() == ROLE_Authority)
		_Server_ParseSkillAnimInfo();
}

/**
 *  Skill Animation을 Play.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if ( InSkillAnimInfo.SkillAnimServerKey == Skill::G_InvalidSkillAnimKey ||
		!AnimPlayServerState.IsValidIndex( InSkillAnimInfo.SkillAnimServerKey ))
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return false;
	}

	if ( !ensureMsgf( IsValid(InSkillAnimInfo.SkillAnim), TEXT("Skill Anim is nullptr.") ) )
		return false;

	// Server에서 이 Anim을 이미 Play 중인지 확인
	if ( AnimPlayServerState[InSkillAnimInfo.SkillAnimServerKey] )
		return false;
	
	// ROLE_AutonomousProxy인 경우 로컬에서 플레이 후 Server RPC 전송
	if( GetOwnerRole() == ROLE_AutonomousProxy )
	{
		IR4AnimationInterface* owner = Cast<IR4AnimationInterface>(GetOwner());
		if ( owner == nullptr )
		{
			LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
			return false;
		}

		// Local에서 Play
		owner->PlayAnim_Local( InSkillAnimInfo.SkillAnim, NAME_None, 1.f );
		FAnimMontageInstance* montageInstance = owner->GetActiveInstanceForMontage( InSkillAnimInfo.SkillAnim );
		if(montageInstance == nullptr)
		{
			LOG_ERROR( R4Skill, TEXT("FAnimMontageInstance is nullptr") )
			return false;
		}

		// Anim Play End시 로직 설정
		// this capture, use weak lambda
		montageInstance->OnMontageEnded.BindWeakLambda( this,
			[this, &InSkillAnimInfo, instanceId = montageInstance->GetInstanceID()]
			(UAnimMontage* InMontage, bool InIsInterrupted)
			{
				OnEndSkillAnim( instanceId, InSkillAnimInfo, InIsInterrupted );
			} );
		
		OnBeginSkillAnim( montageInstance->GetInstanceID(), InSkillAnimInfo );
	}

	_ServerRPC_PlaySkillAnim( InSkillAnimInfo.SkillAnimServerKey, R4GetServerTimeSeconds( GetWorld() ));
	return true;
}

/**
 * Anim을 Play시작 시 호출. Server와 Owner Client 에서 호출.
 * @param InInstanceID : 부여된 MontageInstanceID
 * @param InSkillAnimInfo : Play될 Skill Anim 정보
 */
void UR4AnimSkillBase::OnBeginSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo )
{
	// DetectNotify <-> Effect Bind
	_BindSkillAnimNotifies( InInstanceID, InSkillAnimInfo );
}

/**
 *  Anim 종료 시 호출. Server와 Owner Client 에서 호출
 * @param InInstanceID : Play시 부여된 MontageInstanceID
 * @param InSkillAnimInfo : End될 Skill Anim 정보
 */
void UR4AnimSkillBase::OnEndSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted )
{
	// DetectNotify <-> Effect Unbind
	_UnbindSkillAnimNotifies( InInstanceID, InSkillAnimInfo );
}

/**
 *  Server RPC의 Validation check 시 사용.
 *  기본적으로 SkillAnimPlayState내 유효한 Index (1 ~ AnimCount 만큼의 Key인 경우) 확인
 */
bool UR4AnimSkillBase::PlaySkillAnim_Validate( uint32 InSkillAnimKey ) const
{
	// SkillAnimPlayState내 유효한 Index (1 ~ AnimCount 만큼의 Key인 경우) 요청 시 Validate 성공
	return (InSkillAnimKey != Skill::G_InvalidSkillAnimKey)
		&& AnimPlayServerState.IsValidIndex( InSkillAnimKey );
}

/**
 *  Server RPC의 Play Skill Anim 시 요청 무시 check에 사용.
 *  기본적으로 SkillAnimPlayState[InSkillAnimKey]를 통해 Server에서 Play 중인 Skill Anim인지 확인
 *  (Validate Check 후 Server RPC 내에서 체크함으로 Index가 유효함이 보장)
 */
bool UR4AnimSkillBase::PlaySkillAnim_Ignore( uint32 InSkillAnimKey ) const
{
	// 사용 중인데 또 요청 보낸 경우 무시
	return AnimPlayServerState[InSkillAnimKey];
}

/**
 *  Key에 맞는 Skill Anim이 Server에서 Play되고 있는지 확인 
 */
bool UR4AnimSkillBase::IsSkillAnimPlaying( uint32 InSkillAnimKey ) const
{
	if(InSkillAnimKey == Skill::G_InvalidSkillAnimKey ||
		!AnimPlayServerState.IsValidIndex( InSkillAnimKey ))
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return false;
	}

	return AnimPlayServerState[InSkillAnimKey];
}

/**
 *  Server로 Animation Play를 요청.
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
void UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Implementation( uint32 InSkillAnimKey, float InStartServerTime )
{
	auto it = Server_CachedSkillAnimInfo.Find( InSkillAnimKey );
	if(it == nullptr)
	{
		LOG_WARN( R4Skill, TEXT("InSkillKey [%d] is invalid. check replicate state."), InSkillAnimKey );
		return;
	}

	if ( !ensureMsgf( IsValid( (*it)->SkillAnim ), TEXT("Skill Anim is nullptr.") ) )
		return;

	// 특정 경우 요청을 무시.
	if ( PlaySkillAnim_Ignore( InSkillAnimKey ) )
		return;
	
	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>( GetOwner() );
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return;
	}
	
	// Autonomous 제외한 나머지 Animation Play.
	owner->Server_PlayAnim_WithoutAutonomous( (*it)->SkillAnim, NAME_None, 1.f, true, InStartServerTime );
	FAnimMontageInstance* montageInstance = owner->GetActiveInstanceForMontage( (*it)->SkillAnim );
	if(montageInstance == nullptr)
	{
		LOG_WARN( R4Skill, TEXT("FAnimMontageInstance is nullptr. Failed to play anim.") )
		return;
	}

	// Anim Play End시 로직 설정
	// this capture, use weak lambda
	montageInstance->OnMontageEnded.BindWeakLambda( this,
		[this, &InSkillAnimInfo = **it, instanceId = montageInstance->GetInstanceID()]
		(UAnimMontage* InMontage, bool InIsInterrupted)
		{
			// Skill Anim 사용 상태 해제
			AnimPlayServerState[InSkillAnimInfo.SkillAnimServerKey] = false;
			OnEndSkillAnim( instanceId, InSkillAnimInfo, InIsInterrupted );
		} );

	// Skill Anim 사용 상태 설정
	AnimPlayServerState[InSkillAnimKey] = true;
	OnBeginSkillAnim( montageInstance->GetInstanceID(), **it );
}

/**
 *  Server Animation Play를 요청의 유효성을PlaySkillAnim_Validate로 확인.
 *  PlaySkillAnim_Validate() 을 Override하여 키에 따라 로직 작성
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
bool UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Validate( uint32 InSkillAnimKey, float InStartServerTime  )
{
	return PlaySkillAnim_Validate( InSkillAnimKey );
}

/**
 *  Notify Buff를 적용
 *  @param InNotifyBuff : 적용할 Notify Buff
 */
void UR4AnimSkillBase::_ApplyNotifyBuffs( const FR4NotifyBuffWrapper& InNotifyBuff )
{
	if ( !IsValid( InNotifyBuff.BuffClass ) )
	{
		LOG_WARN( R4Skill, TEXT("Notify Buff is Invalid.") );
		return;
	}

	// simulated 인 경우, Owner이지만 Owner까지 적용할 필요가 없는 경우 무시
	if ( GetOwnerRole() == ROLE_SimulatedProxy
		|| ( GetOwnerRole() == ROLE_AutonomousProxy && !InNotifyBuff.bApplyOwner ) )
		return;
	
	// 나에게 버프 적용
	if ( IR4BuffReceiveInterface* victim = Cast<IR4BuffReceiveInterface>( GetOwner() ) )
		victim->ReceiveBuff( GetOwner(), InNotifyBuff.BuffClass, InNotifyBuff.BuffSetting );
}

/**
 *  InMontageInstanceId를 Key로 SkillAnim에 특정한 Notify bind
 *  @param InMontageInstanceId : Notify delegate bind 시 구별할 MontageInstance ID
 *  @param InSkillAnimInfo : FR4SkillAnimInfo
 */
void UR4AnimSkillBase::_BindSkillAnimNotifies( int32 InMontageInstanceId, const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ) , TEXT("Skill Anim is Invalid.")))
		return;

	// bind detect notify
	for ( const auto& [notifyIndex, detectEffect] : InSkillAnimInfo.DetectNotifies )
	{
		if( !ensureMsgf( InSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( notifyIndex ),
			TEXT("Notify index is Invalid.")))
			return;

		if( IR4NotifyByIdInterface* detectNotifyObj = Cast<IR4NotifyByIdInterface>(InSkillAnimInfo.SkillAnim->Notifies[notifyIndex].Notify ) )
		{
			// this 캡처, WeakLambda 사용
			detectNotifyObj->OnNotify( InMontageInstanceId ).BindWeakLambda(this,
				[this, &detectEffect]()
				{
					// 탐지 실행
					ExecuteDetect( detectEffect );
				});
		}
	}

	// bind buff notify
	for ( auto buffIt = InSkillAnimInfo.BuffNotifies.CreateConstIterator(); buffIt; ++buffIt )
	{
		if( !ensureMsgf( InSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( buffIt->NotifyNumber ),
			TEXT("Notify index is Invalid.")))
			return;

		if ( IR4NotifyByIdInterface* buffNotifyObj = Cast<IR4NotifyByIdInterface>( InSkillAnimInfo.SkillAnim->Notifies[buffIt->NotifyNumber].Notify ) )
		{
			// this 캡처, WeakLambda 사용
			buffNotifyObj->OnNotify( InMontageInstanceId ).BindWeakLambda(this,
				[this, &notifyBuff = (*buffIt)]()
				{
					// 버프 적용
					_ApplyNotifyBuffs( notifyBuff );
				});
		}
	}
}

/**
 *  InMontageInstanceId를 Key로 Bind해 두었던 SkillAnim에 특정한 Notify unbind
 *  @param InMontageInstanceId : Notify delegate bind 시 구별할 MontageInstance ID
 *  @param InSkillAnimInfo : FR4SkillAnimInfo
 */
void UR4AnimSkillBase::_UnbindSkillAnimNotifies( int32 InMontageInstanceId, const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ), TEXT("Skill Anim is Invalid.")))
		return;

	// unbind detect notify
	for ( const auto& [notifyIndex, detectEffect] : InSkillAnimInfo.DetectNotifies )
	{
		if( !ensureMsgf( InSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( notifyIndex ),
			TEXT("Notify index is Invalid.")))
			return;

		if( IR4NotifyByIdInterface* detectNotifyObj = Cast<IR4NotifyByIdInterface>(InSkillAnimInfo.SkillAnim->Notifies[notifyIndex].Notify ) )
			detectNotifyObj->UnbindNotify( InMontageInstanceId );
	}

	// unbind buff notify
	for ( auto buffIt = InSkillAnimInfo.BuffNotifies.CreateConstIterator(); buffIt; ++buffIt )
	{
		if( !ensureMsgf( InSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( buffIt->NotifyNumber ),
			TEXT("Notify index is Invalid.")))
			return;

		if ( IR4NotifyByIdInterface* buffNotifyObj = Cast<IR4NotifyByIdInterface>( InSkillAnimInfo.SkillAnim->Notifies[buffIt->NotifyNumber].Notify ) )
			buffNotifyObj->UnbindNotify( InMontageInstanceId );
	}
}

/**
 *  Server에서 Skill Anim 멤버를 찾아서 Skill Anim 키 부여
 */
void UR4AnimSkillBase::_Server_ParseSkillAnimInfo()
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server") ) )
		return;
	
	for ( const auto& [prop, value] : TPropertyValueRange<FStructProperty>( GetClass(), this ) )
	{
		if ( prop->Struct != FR4SkillAnimInfo::StaticStruct() )
			continue;
		
		const FR4SkillAnimInfo* c_animInfoPtr = static_cast<const FR4SkillAnimInfo*>(value);
		if ( !ensureMsgf( IsValid(c_animInfoPtr->SkillAnim), TEXT("Skill Anim is invalid.") ) )
			return;

		// 키값 부여 & 캐싱
		if ( FR4SkillAnimInfo* animInfoPtr = const_cast<FR4SkillAnimInfo*>(c_animInfoPtr) )
		{
			animInfoPtr->SkillAnimServerKey = ++Server_CachedSkillAnimInfoCount;
			Server_CachedSkillAnimInfo.Emplace( animInfoPtr->SkillAnimServerKey, c_animInfoPtr );
		}
	}

	// SkillAnimPlayState를 Skill Anim만큼 초기화
	AnimPlayServerState.SetNum( Server_CachedSkillAnimInfoCount + 1 );
}