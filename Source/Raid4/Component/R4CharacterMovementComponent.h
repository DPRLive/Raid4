// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include "R4CharacterMovementComponent.generated.h"

/**
 * Character가 사용할 R4Character Movement Component.
 */
UCLASS()
class RAID4_API UR4CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UR4CharacterMovementComponent();

	// 컴포넌트를 초기화 한다.
	virtual void InitializeComponent() override;

private:
	// 가속 -> 최고 속도 / 감속 -> 0 에 도달할때 걸리는 시간(seconds) 설정.
	UPROPERTY( EditAnywhere, Category = "Setting", meta = (AllowPrivateAccess = true, ClampMin = "0.001", UIMin = "0.001") )
	float AccelerationTime;
};
