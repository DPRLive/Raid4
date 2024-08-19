// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4CharacterRPCComponent.generated.h"

class UAnimMontage;

USTRUCT()
struct FPlayAnimInfo
{
	GENERATED_BODY()

	FPlayAnimInfo(UAnimMontage* InAnimMontage = nullptr, int32 InSectionIndex = INDEX_NONE, float InPlayRate = 1.f, float InServerStartTime = 0.f)
		: AnimMontage(InAnimMontage)
		, SectionIndex(InSectionIndex)
		, PlayRate(InPlayRate)
		, StartServerTime(InServerStartTime)
	{ }
	
	// 플레이할 애니메이션
	UPROPERTY()
	TWeakObjectPtr<UAnimMontage> AnimMontage;

	// Section index
	UPROPERTY()
	int32 SectionIndex;
	
	// PlayRate
	UPROPERTY()
	float PlayRate;

	// 서버기준 시작한 시간
	UPROPERTY()
	float StartServerTime;
};

/**
 * Character에 필요한 RPC들을 모아둔 Component
 */
UCLASS( ClassGroup=(RPC), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4CharacterRPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4CharacterRPCComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

private:
	// Initialize
	virtual void AfterBeginPlay();
	
public:
	// Animation 을 플레이한다. AnimMontage의 링크를 포함한 특정 Section Anim 1개의 몽타주 플레이 동기화 보장
	float PlayAnim(UAnimMontage* InAnimMontage, const FName& InStartSectionName = NAME_None, float InPlayRate = 1.f);

	// 모든 Anim Montage를 Stop한다.
	void StopAllAnim();
	
private:
	// Server로 Animation 플레이를 요청
	UFUNCTION( Server, Reliable )
	void _ServerRPC_PlayAnim(const FPlayAnimInfo& InAnimInfo);

	// Server로 Anim All Stop을 요청
	UFUNCTION( Server, Reliable )
	void _ServerRPC_StopAllAnim();
	
	// Replicate 된 Anim Info를 처리
	UFUNCTION()
	void _OnRep_AnimInfo(const FPlayAnimInfo& InPrevAnimInfo);

private:
	// 전달 받은 Animation의 정보 (Simulated Proxy만 받음)
	UPROPERTY( ReplicatedUsing = _OnRep_AnimInfo )
	FPlayAnimInfo RepAnimInfo;
};