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

protected:
	// 바닥에서 움직이는 속도를 체크
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float GroundSpeed;

	// Z축으로의 속도
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float ZSpeed;
	
	// 이동 방향 각도를 체크
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float DirectionAngle;
	
	// 공중에 있는지
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	uint8 bIsInAir:1;

	// 가속 중?
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	uint8 bIsAccelerating:1;
	
	// Character와 Aim 사이 Rotation
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	FRotator CharacterToAimRotation;

	// YawDelta for lean
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float YawDelta;

	// Is Full Body?
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	uint8 bFullBody:1;

	// Turn in place. Delta Yaw를 누적시키고 Turn In Place Speed로 Interpolate하는 방식
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	float TurnInPlace;
	
protected:
	// Lean Intensity Scaling Setting
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Character|Setting" )
	float LeanScaling;

	// Turn in place interpolate speed
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Character|Setting" )
	float TurnInPlaceInterpSpeed;

protected:
	// 이 애니메이션의 Owner
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	TWeakObjectPtr<ACharacter> Owner;

	// Move Comp
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Character" )
	TWeakObjectPtr<UCharacterMovementComponent> MovementComp;
	
	// Yaw delta 보간을 위한 마지막 Rotation 캐싱
	FRotator PrevRotation;
};
