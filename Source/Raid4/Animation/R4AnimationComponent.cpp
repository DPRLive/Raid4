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
	
	DOREPLIFETIME_CONDITION(UR4AnimationComponent, RepAnimInfo, COND_SimulatedOnly);
}

void UR4AnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([thisPtr = TWeakObjectPtr<UR4AnimationComponent>(this)]()
	{
		if(thisPtr.IsValid())
			thisPtr->AfterBeginPlay();
	});
}

/**
 *  BeginPlay가 실행된 이후에 실행 (BeginPlay 후 다음 Tick에 실행되는 함수)
 */
void UR4AnimationComponent::AfterBeginPlay()
{
	// Server Time second가 AGameState의 BeginPlay 시점이 되어야 적용 되어서 여기서 실행
	if(RepAnimInfo.AnimMontage.IsValid())
		_OnRep_AnimInfo(FPlayAnimInfo());
}

/**
 *  Server에서, Authority와 Autonomous Proxy를 제외하고 AnimPlay를 명령. ServerTime 조정으로 동기화 지원
 *  @param InAnimMontage : Play할 Anim Montage
 *  @param InStartSectionName : Play할 Anim Section의 Name
 *  @param InPlayRate : PlayRate
 *  @param InIsWithServer : Server도 같이 Play가 필요한지?
 *  @param InServerTime : 이 Animation을 Play한 서버 시작 시간 
 *  @return : AnimMontage의 링크를 포함한 특정 Section에 대한 시간, 루프시 -1 반환
 */
float UR4AnimationComponent::Server_PlayAnim_WithoutAutonomous(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, bool InIsWithServer, float InServerTime)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")) ||
		!IsValid(InAnimMontage))
		return 0.f;

	// COND_SimulatedOnly Property Replication으로 동기화.
	RepAnimInfo.AnimMontage = InAnimMontage;
	RepAnimInfo.PlayRate = InPlayRate;
	RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndex(InStartSectionName);

	if(RepAnimInfo.SectionIndex == INDEX_NONE) // INDEX가 NONE이면 시작 Section Index를 설정
		RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndexFromPosition(0);

	if(FMath::IsNearlyEqual(InServerTime, -1.f))
		RepAnimInfo.StartServerTime = R4GetServerTimeSeconds(GetWorld());
	else
		RepAnimInfo.StartServerTime = InServerTime;

	// 필요 시 Server에서도 플레이
	if(InIsWithServer)
		_PlayAnimSync(RepAnimInfo);
	
	return UtilAnimation::GetCompositeAnimLength(InAnimMontage, RepAnimInfo.SectionIndex);
}

/**
 *  Server에서, Authority와 Autonomous Proxy를 제외하고 AnimStop을 명령.
 *  @param InIsWithServer : Server도 같이 Stop해야 하는지?
 */
void UR4AnimationComponent::Server_StopAnim_WithoutAutonomous(bool InIsWithServer)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	// nullptr을 지정하여 stop
	RepAnimInfo = nullptr;

	// 필요시 서버도 같이 Stop
	if(InIsWithServer)
		_PlayAnimSync(RepAnimInfo);
}

/**
 *  delay를 적용하여 동기화 된 Animation Play.
 *  @param InRepAnimInfo : Replicate 된 Anim 정보
 */
void UR4AnimationComponent::_PlayAnimSync(const FPlayAnimInfo& InRepAnimInfo) const
{
	ACharacter* owner = Cast<ACharacter>(GetOwner());
	if(!IsValid(owner))
		return;
	
	UAnimInstance* anim = IsValid(owner->GetMesh()) ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if(!IsValid(anim))
		return;
		
	// nullptr이면, 정지
	if(!InRepAnimInfo.AnimMontage.IsValid())
	{
		anim->StopAllMontages(anim->Montage_GetBlendTime(nullptr));
		return;
	}

	// 아니면 플레이
	float serverTime = R4GetServerTimeSeconds(GetWorld());
	if(FMath::IsNearlyEqual(serverTime, -1.f))
		return;
	
	// 딜레이를 적용
	// TODO : Server -> Client 상 pkt lag이 있다면, 보정이 힘들 수 있음. ( ServerWorldTimeSecond도 Replicate 되기 때문 )
	float delayTime = serverTime - InRepAnimInfo.StartServerTime;
	delayTime = FMath::Clamp(delayTime, 0.f, delayTime);
	
	float startPos = UtilAnimation::GetDelayedStartAnimPos(InRepAnimInfo.AnimMontage.Get(), InRepAnimInfo.SectionIndex, delayTime); 
	
	if(!FMath::IsNearlyEqual(startPos, -1))
		anim->Montage_Play(InRepAnimInfo.AnimMontage.Get(), InRepAnimInfo.PlayRate, EMontagePlayReturnType::MontageLength, startPos);
}

/**
 *  Replicate 된 Anim Info를 처리 ( 서버 시간을 참고하여 동시에 끝날 수 있도록 딜레이를 고려한 상태에서 시작 )
 */
void UR4AnimationComponent::_OnRep_AnimInfo(const FPlayAnimInfo& InPrevAnimInfo)
{
	_PlayAnimSync(RepAnimInfo);
}