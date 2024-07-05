// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterRPCComponent.h"

#include <Net/UnrealNetwork.h>
#include <GameFramework/Character.h>
#include <GameFramework/GameStateBase.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterRPCComponent)

#pragma region GLOBAL_FUNC

/**
 *  AnimMontage의 링크를 포함한 특정 Section에 대한 총 시간을 측정
 *  @param InAnimMontage : 시간을 측정할 대상 Anim Montage
 *  @param InSectionIndex : 시간을 측정할 Section의 시작 index
 *  @return : 총 시간, 루프시 -1 반환
 */
float GetCompositeAnimLength(UAnimMontage* InAnimMontage, int32 InSectionIndex)
{
	if(InAnimMontage == nullptr || InSectionIndex == INDEX_NONE)
		return 0.f;

	float totalLength = 0.f;
	
	TArray<bool> Chk;
	Chk.SetNum(InAnimMontage->GetNumSections());

	int32 nextIndex = InSectionIndex;
	do
	{
		Chk[nextIndex] = true;

		// 길이 갱신
		totalLength += InAnimMontage->GetSectionLength(nextIndex);
		
		// Next Section 검색
		const FName& nextSection = InAnimMontage->GetAnimCompositeSection(nextIndex).NextSectionName;
		nextIndex = InAnimMontage->GetSectionIndex(nextSection);
		
		if(Chk.IsValidIndex(nextIndex) && Chk[nextIndex]) // 사이클 탐지시 -1.f 반환
			return -1.f;
	}
	while(nextIndex != INDEX_NONE); // 다음 링크가 없을때 까지.

	return totalLength;
}

/**
 *  링크를 포함한 anim montage section의 delay된 시작 Pos 반환
 *  @param InAnimMontage : 대상 Anim Montage
 *  @param InSectionIndex : Section의 시작 index
 *  @param InDelay : 적용할 delay
 *  @return : delay가 적용된 시작 Pos, (InAnimMontage 가 null이거나, InSectionIndex가 INDEX_NONE이거나, delay가 총 길이보다 길면 -1 반환)
 */
float GetDelayedStartAnimPos(UAnimMontage* InAnimMontage, int32 InSectionIndex, float InDelay)
{
	if(InAnimMontage == nullptr || InSectionIndex == INDEX_NONE)
		return -1.f;

	float startPos = InAnimMontage->GetAnimCompositeSection(InSectionIndex).GetTime();
	float nowLength = InAnimMontage->GetSectionLength(InSectionIndex);
	int32 nowIndex = InSectionIndex;
	
	while(InDelay > nowLength)
	{
		InDelay -= nowLength;
		
		// Next Section 검색
		const FName& nextSection = InAnimMontage->GetAnimCompositeSection(nowIndex).NextSectionName;

		// delay가 아직 남았는데 section이 끝나버리면 -1 반환
		if(nextSection == NAME_None)
			return -1.f;

		nowIndex = InAnimMontage->GetSectionIndex(nextSection);
		startPos = InAnimMontage->GetAnimCompositeSection(nowIndex).GetTime();
		nowLength = InAnimMontage->GetSectionLength(nowIndex);
	}

	return startPos + InDelay;
}

#pragma endregion GLOBAL_FUNC

UR4CharacterRPCComponent::UR4CharacterRPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	RepAnimInfo = FPlayAnimInfo();
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4CharacterRPCComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UR4CharacterRPCComponent, RepAnimInfo, COND_SimulatedOnly);
}

void UR4CharacterRPCComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([thisPtr = TWeakObjectPtr<UR4CharacterRPCComponent>(this)]()
	{
		if(thisPtr.IsValid())
			thisPtr->AfterBeginPlay();
	});
}

/**
 *  BeginPlay가 실행된 이후에 실행 (BeginPlay 후 다음 Tick에 실행되는 함수)
 */
void UR4CharacterRPCComponent::AfterBeginPlay()
{
	// Server Time second가 AGameState의 BeginPlay 시점이 되어야 적용 되어서 여기서 실행
	if(RepAnimInfo.AnimMontage.IsValid())
		_OnRep_AnimInfo(FPlayAnimInfo());
}

/**
 *  Animation을 플레이, AnimMontage의 링크를 포함한 특정 Section Anim 1개의 몽타주 플레이 동기화 보장
 *  @return : AnimMontage의 링크를 포함한 특정 Section에 대한 시간, 루프시 -1 반환
 */
float UR4CharacterRPCComponent::PlayAnim(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate)
{
	if(InAnimMontage == nullptr)
		return 0.f;

	FPlayAnimInfo prevValue = RepAnimInfo;
	RepAnimInfo.AnimMontage = InAnimMontage;
	RepAnimInfo.PlayRate = InPlayRate;
	RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndex(InStartSectionName);

	if(RepAnimInfo.SectionIndex == INDEX_NONE) // INDEX가 NONE이면 시작 Section Index를 설정
		RepAnimInfo.SectionIndex = InAnimMontage->GetSectionIndexFromPosition(0);
	
	if(AGameStateBase* gameState = (IsValid(GetWorld()) ? GetWorld()->GetGameState() : nullptr) ; IsValid(gameState))
		RepAnimInfo.StartServerTime = gameState->GetServerWorldTimeSeconds();
	
	// Owner 인 경우 Play
	if(GetOwnerRole() == ROLE_AutonomousProxy || GetOwnerRole() == ROLE_Authority)
	{
		_OnRep_AnimInfo(prevValue);
		
		// 서버가 아닌경우 서버로 요청을 전송
		if(GetOwnerRole() != ROLE_Authority)
			_ServerRPC_PlayAnim(RepAnimInfo);
	}
	
	return GetCompositeAnimLength(InAnimMontage, RepAnimInfo.SectionIndex);
}

/**
 * 모든 Animation 을 Stop한다.
 */
void UR4CharacterRPCComponent::StopAllAnim()
{
	if(!RepAnimInfo.AnimMontage.IsValid())
		return;
	
	// Owner 인 경우 우선 Stop
	if(GetOwnerRole() == ROLE_AutonomousProxy || GetOwnerRole() == ROLE_Authority)
	{
		FPlayAnimInfo prevValue = RepAnimInfo;

		RepAnimInfo = nullptr; // nullptr로 변경 후 요청
		_OnRep_AnimInfo(prevValue);
		
		// 서버가 아닌경우 서버로 요청을 전송
		if(GetOwnerRole() != ROLE_Authority)
			_ServerRPC_StopAllAnim();
	}
}

/**
 *  Server로 Animation 플레이를 요청
 */
void UR4CharacterRPCComponent::_ServerRPC_PlayAnim_Implementation(const FPlayAnimInfo& InAnimInfo)
{
	FPlayAnimInfo prevValue = RepAnimInfo;
	RepAnimInfo = InAnimInfo; // Simulated Proxy에게만 전달.
	_OnRep_AnimInfo(prevValue);
}

/**
 *  Server로 Anim Stop을 요청, 모든 Anim Montage를 Stop
 *  */
void UR4CharacterRPCComponent::_ServerRPC_StopAllAnim_Implementation()
{
	FPlayAnimInfo prevValue = RepAnimInfo;
	RepAnimInfo.AnimMontage = nullptr; // Simulated Proxy에게만 전달.
	_OnRep_AnimInfo(prevValue);
}

/**
 *  Replicate 된 Anim Info를 처리 ( 서버 시간을 참고하여 동시에 끝날 수 있도록 딜레이를 고려한 상태에서 시작 )
 */
void UR4CharacterRPCComponent::_OnRep_AnimInfo(const FPlayAnimInfo& InPrevAnimInfo)
{
	ACharacter* owner = Cast<ACharacter>(GetOwner());
	if(!IsValid(owner))
		return;
	
	UAnimInstance* anim = owner->GetMesh() ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if(!IsValid(anim))
		return;
		
	// nullptr이면, 정지
	if(!RepAnimInfo.AnimMontage.IsValid())
	{
		float blendOutTime = 0.f;
		if(InPrevAnimInfo.AnimMontage.IsValid())
			blendOutTime = InPrevAnimInfo.AnimMontage->BlendOut.GetBlendTime();

		anim->StopAllMontages(blendOutTime);
		return;
	}

	// 아니면 플레이
	AGameStateBase* gameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if(!IsValid(gameState))
		return;
	
	// RPC로 인한 딜레이를 적용
	// TODO : Server -> Client 상 pkt lag이 있다면, 보정이 힘들 수 있음. ( ServerWorldTimeSecond도 Replicate 되기 때문 )
	float delayTime = gameState->GetServerWorldTimeSeconds() - RepAnimInfo.StartServerTime;
	delayTime = FMath::Clamp(delayTime, 0.f, delayTime);
	
	float startPos = GetDelayedStartAnimPos(RepAnimInfo.AnimMontage.Get(), RepAnimInfo.SectionIndex, delayTime); 
	
	if(!FMath::IsNearlyEqual(startPos, -1))
		anim->Montage_Play(RepAnimInfo.AnimMontage.Get(), RepAnimInfo.PlayRate, EMontagePlayReturnType::MontageLength, startPos);
}