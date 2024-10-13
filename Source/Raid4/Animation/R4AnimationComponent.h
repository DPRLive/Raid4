// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>

#include "R4AnimationComponent.generated.h"

class UAnimMontage;

/**
 * Animation RPC, 동기화등 Anim 관련 기능 Comp.
 * 한개의 AnimMontage에 대해 ServerTime 조정으로 동기화 가능
 * 현재 음수의 play rate는 처리되지 않음.
 */
UCLASS( ClassGroup=(Animation), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4AnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4AnimationComponent();

public:
	// ServerTime을 통한 동기화된 Animation Play를 지원. ( InServerTime 기준으로 보정 )
	void PlayAnimSync( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, float InStartServerTime ) const;

	// 설정해둔 Dead Animation을 출력.
	void PlayDeadAnim();

	FORCEINLINE void SetDeadAnim( const TObjectPtr<UAnimMontage>& InDeadAnim ) { DeadAnim = InDeadAnim; }
private:
	// Dead Anim
	UPROPERTY( EditAnywhere )
	TObjectPtr<UAnimMontage> DeadAnim;
};