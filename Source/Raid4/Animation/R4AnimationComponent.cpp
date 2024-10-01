// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimationComponent.h"
#include "../Util/UtilAnimation.h"

#include <Net/UnrealNetwork.h>
#include <GameFramework/Character.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimationComponent)

UR4AnimationComponent::UR4AnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	RepAnimInfo = FPlayAnimInfo();
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4AnimationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION( UR4AnimationComponent, RepAnimInfo, COND_SimulatedOnly );
}

void UR4AnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<UR4AnimationComponent>( this )]()
	{
		if ( thisPtr.IsValid() )
			thisPtr->AfterBeginPlay();
	} );
}

/**
 *  BeginPlay가 실행된 이후에 실행 (BeginPlay 후 다음 Tick에 실행되는 함수)
 */
void UR4AnimationComponent::AfterBeginPlay()
{
	// Server Time second가 AGameState의 BeginPlay 시점이 되어야 적용 되어서 여기서 실행
	if ( RepAnimInfo.AnimMontage.IsValid() )
		_OnRep_AnimInfo( FPlayAnimInfo() );
}

/**
 *  Server에서, Authority와 Autonomous Proxy를 제외하고 AnimPlay를 명령. ServerTime 조정으로 동기화 지원
 *  @param InAnimMontage : Play할 Anim Montage
 *  @param InStartSectionName : Play할 Anim Section의 Name
 *  @param InPlayRate : PlayRate, 현재 음수의 play rate는 처리하지 않음.
 *  @param InIsWithServer : Server도 같이 Play가 필요한지?
 *  @param InServerTime : 이 Animation을 Play한 서버 시작 시간 
 */
void UR4AnimationComponent::Server_PlayAnim_WithoutAutonomous(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, bool InIsWithServer, float InServerTime)
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.") ) ||
		!IsValid( InAnimMontage ) )
		return;
	
	if( InPlayRate < 0.f )
	{
		LOG_WARN( R4Anim, TEXT("Negative playrate [%f] is not processed."), RepAnimInfo.PlayRate );
		return;
	}
	
	FPlayAnimInfo prevRepAnimInfo = RepAnimInfo;
	
	// COND_SimulatedOnly Property Replication으로 동기화.
	RepAnimInfo.AnimMontage = InAnimMontage;
	RepAnimInfo.PlayRate = InPlayRate;
	RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndex( InStartSectionName );

	if ( RepAnimInfo.SectionIndex == INDEX_NONE ) // INDEX가 NONE이면 시작 Section Index를 설정
		RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndexFromPosition( 0 );

	if ( FMath::IsNearlyEqual( InServerTime, -1.f ) )
		RepAnimInfo.StartServerTime = R4GetServerTimeSeconds( GetWorld() );
	else
		RepAnimInfo.StartServerTime = InServerTime;

	// 필요 시 Server에서도 플레이
	if ( InIsWithServer )
		_PlayAnimSync( prevRepAnimInfo, RepAnimInfo );
}

/**
 *  Server에서, Authority와 Autonomous Proxy를 제외하고 Section Jump 명령. ServerTime 조정으로 동기화 지원
 *  @param InSectionName : Jump할 Anim Section의 Name
 *  @param InIsWithServer : Server도 같이 Jump가 필요한지?
 *  @param InServerTime : 이 Animation을 Play한 서버 시작 시간 
 */
void UR4AnimationComponent::Server_JumpToSection_WithoutAutonomous( const FName& InSectionName, bool InIsWithServer, float InServerTime )
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.") ) ||
		!IsValid( RepAnimInfo.AnimMontage.Get() ) )
		return;

	int32 nextSectionIndex = RepAnimInfo.AnimMontage->GetSectionIndex( InSectionName );
	if ( RepAnimInfo.SectionIndex == INDEX_NONE ) // INDEX가 NONE이면 Jump하지 않음
	{
		LOG_WARN( R4Data, TEXT("InSectionName is invalid. : [%s]"), *InSectionName.ToString() )
		return;
	}

	FPlayAnimInfo prevRepAnimInfo = RepAnimInfo;

	// COND_SimulatedOnly Property Replication으로 동기화. (Anim Montage는 변경하지 않음)
	RepAnimInfo.SectionIndex = nextSectionIndex;

	if(FMath::IsNearlyEqual(InServerTime, -1.f))
		RepAnimInfo.StartServerTime = R4GetServerTimeSeconds(GetWorld());
	else
		RepAnimInfo.StartServerTime = InServerTime;

	// 필요 시 Server에서도 Jump할 수 있도록 
	if ( InIsWithServer )
		_PlayAnimSync( prevRepAnimInfo, RepAnimInfo );
}

/**
 *  Server에서, Authority와 Autonomous Proxy를 제외하고 AnimStop을 명령.
 *  @param InIsWithServer : Server도 같이 Stop해야 하는지?
 */
void UR4AnimationComponent::Server_StopAnim_WithoutAutonomous( bool InIsWithServer )
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.") ) )
		return;

	// nullptr을 지정하여 stop
	FPlayAnimInfo prevRepAnimInfo = RepAnimInfo;
	RepAnimInfo = nullptr;

	// 필요시 서버도 같이 Stop
	if ( InIsWithServer )
		_PlayAnimSync( prevRepAnimInfo, RepAnimInfo );
}

/**
 *  delay를 적용하여 동기화 된 Animation Play.
 *  현재 음수의 play rate는 처리하지 않음.
 *  <Delay의 처리 방식>
 *  Loop Animation의 경우 : Delay 된 StartPos에서 시작
 *  일반 Animation의 경우 : PlayRate를 보정하여 동일 시점에 끝나도록 보정, ( delay > anim length인 경우 : Skip play )
 *  @param InPrevRepAnimInfo : 이전 Replicate 된 Anim 정보
 *  @param InNowRepAnimInfo : 현재 Replicate 된 Anim 정보
 */
void UR4AnimationComponent::_PlayAnimSync( const FPlayAnimInfo& InPrevRepAnimInfo, const FPlayAnimInfo& InNowRepAnimInfo ) const
{
	ACharacter* owner = Cast<ACharacter>( GetOwner() );
	if ( !IsValid( owner ) )
		return;

	UAnimInstance* anim = IsValid( owner->GetMesh() ) ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if ( !IsValid( anim ) )
		return;

	// nullptr이면 정지, 아니면 플레이
	if ( !InNowRepAnimInfo.AnimMontage.IsValid() )
	{
		anim->StopAllMontages( anim->Montage_GetBlendTime( nullptr ) );
		return;
	}

	if( RepAnimInfo.PlayRate < 0.f )
	{
		LOG_WARN( R4Anim, TEXT("Negative playrate [%f] is not processed."), RepAnimInfo.PlayRate );
		return;
	}
	
	// TODO : Server -> Client 상 pkt lag이 있다면, 보정이 힘들 수 있음. ( ServerWorldTimeSecond도 Replicate 되기 때문 )
	float serverTime = R4GetServerTimeSeconds( GetWorld() );
	float delayTime = FMath::Max( 0.f, serverTime - InNowRepAnimInfo.StartServerTime );
	
	float animTime = UtilAnimation::GetCompositeAnimLength( InNowRepAnimInfo.AnimMontage.Get(), RepAnimInfo.SectionIndex );
	float playRate = FMath::Max( KINDA_SMALL_NUMBER, InNowRepAnimInfo.PlayRate);
	float startPos = 0.f;
	bool bLoop = FMath::IsNearlyEqual( animTime, -1.f );
	
	// 실제 play되는 anim 시간을 구하기 위해 PlayRate 적용
	animTime /= playRate;
	
	// Loop Animation의 경우 : Delay 된 StartPos에서 시작
	// 일반 Animation의 경우 : 동일 시점에 끝나도록 PlayRate를 보정
	if(bLoop)
	{
		startPos = UtilAnimation::GetDelayedStartAnimPos( InNowRepAnimInfo.AnimMontage.Get(), InNowRepAnimInfo.SectionIndex, delayTime );
	}
	else if(delayTime < animTime)
	{
		// playRate = 전체시간 / 남은시간
		playRate = animTime / (animTime - delayTime);
		
		startPos = InNowRepAnimInfo.AnimMontage->GetAnimCompositeSection( InNowRepAnimInfo.SectionIndex ).GetTime();
	}
	else // 루프가 아닌데 delay > anim length인 경우 : Skip play 
		return;
	
	// Play인지 Jump인지 Section Jump 확인하여 startPos 적용
	// Jump = 현재 InNowRepAnimInfo Montage가 Play 중이면서 && Montage Section Index가 변경된 경우
	bool bJump = anim->Montage_IsActive( InNowRepAnimInfo.AnimMontage.Get() ) && ( InPrevRepAnimInfo.SectionIndex != InNowRepAnimInfo.SectionIndex );
	if ( bJump )
	{
		// Jump Section (Montage Instance)가 유지됨!
		anim->Montage_SetPosition( InNowRepAnimInfo.AnimMontage.Get(), startPos );
	}
	else
	{
		// 새로 Anim을 Play (Montage Instance가 새로 생성됨)
		anim->Montage_Play( InNowRepAnimInfo.AnimMontage.Get(), InNowRepAnimInfo.PlayRate, EMontagePlayReturnType::MontageLength, startPos );
	}

	// play rate 적용
	anim->Montage_SetPlayRate( InNowRepAnimInfo.AnimMontage.Get(), playRate );
}

/**
 *  Replicate 된 Anim Info를 처리 ( 서버 시간을 참고하여 동시에 끝날 수 있도록 딜레이를 고려한 상태에서 시작 )
 */
void UR4AnimationComponent::_OnRep_AnimInfo(const FPlayAnimInfo& InPrevAnimInfo)
{
	_PlayAnimSync( InPrevAnimInfo, RepAnimInfo );
}
