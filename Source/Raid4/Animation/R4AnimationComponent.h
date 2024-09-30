// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>

#include "R4AnimationComponent.generated.h"

class UAnimMontage;

USTRUCT( Atomic )
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
 * Animation RPC, 동기화등 Anim 관련 기능 Comp.
 * 한개의 AnimMontage에 대해 ServerTime 조정으로 동기화 가능
 */
UCLASS( ClassGroup=(Animation), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4AnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4AnimationComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

private:
	// Initialize
	virtual void AfterBeginPlay();
	
public:
	// Server에서, Autonomous Proxy를 제외하고 AnimPlay를 명령. ServerTime 조정으로 동기화 가능.
	void Server_PlayAnim_WithoutAutonomous( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, bool InIsWithServer, float InServerTime = -1 );

	// Server에서, Autonomous Proxy를 제외하고 Section Jump를 명령. ServerTime 조정으로 동기화 가능.
	void Server_JumpToSection_WithoutAutonomous( const FName& InSectionName, bool InIsWithServer, float InServerTime = -1 );
	
	// Server에서, Autonomous Proxy를 제외하고 AnimStop을 명령.
	void Server_StopAnim_WithoutAutonomous( bool InIsWithServer );
	
private:
	// delay를 적용하여 동기화 된 Animation Play.
	void _PlayAnimSync( const FPlayAnimInfo& InPrevRepAnimInfo, const FPlayAnimInfo& InNowRepAnimInfo ) const;

	// Replicate 된 Anim Info를 처리
	UFUNCTION()
	void _OnRep_AnimInfo( const FPlayAnimInfo& InPrevAnimInfo );

private:
	// 전달 받은 Animation의 정보 (Simulated Proxy만 받음)
	UPROPERTY( ReplicatedUsing = _OnRep_AnimInfo )
	FPlayAnimInfo RepAnimInfo;
};