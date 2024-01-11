// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation/AnimInstance.h>
#include "R4AnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 *  캐릭터들이 사용할 AnimInstance
 */
UCLASS()
class RAID4_API UR4AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UR4AnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 이 애니메이션의 Owner
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	TWeakObjectPtr<ACharacter> Owner;

	// Move Comp
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	TWeakObjectPtr<UCharacterMovementComponent> MovementComp;

	// 땅에서 움직이는 속도를 체크
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float GroundSpeed;

	// Idle 상태인지 확인
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	uint8 bIsIdle : 1;

	// 움직임 판단에 대한 경계값
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Character" )
	float MovingThreshold;
	
	// 떨어지는 상태인지 확인
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	uint8 bIsFalling : 1;
};
