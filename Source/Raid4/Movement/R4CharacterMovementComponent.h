// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include "R4CharacterMovementComponent.generated.h"

/**
 * Character가 사용할 R4Character Movement Component.
 * TODO : 이걸 써? 말아 ?
 */
UCLASS()
class RAID4_API UR4CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UR4CharacterMovementComponent();

	// 컴포넌트를 초기화 한다.
	virtual void InitializeComponent() override;
};
