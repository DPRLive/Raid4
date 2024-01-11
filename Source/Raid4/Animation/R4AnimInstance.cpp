// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimInstance.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimInstance)

UR4AnimInstance::UR4AnimInstance()
{
	GroundSpeed = 0.f;
	bIsIdle = true;
	MovingThreshold = 3.f;
	bIsFalling = false;
}

void UR4AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner.IsValid())
	{
		MovementComp = Owner->GetCharacterMovement();
	}
}

void UR4AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Owner.IsValid() && MovementComp.IsValid())
	{
		GroundSpeed = Owner->GetVelocity().Size2D();
		bIsIdle = (GroundSpeed < MovingThreshold);
		bIsFalling = MovementComp->IsFalling();
	}
}