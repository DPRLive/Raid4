// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimInstance.h"

#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <KismetAnimationLibrary.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimInstance)

UR4AnimInstance::UR4AnimInstance()
{
	GroundSpeed = 0.f;
	ZSpeed = 0.f;
	DirectionAngle = 0.f;
	bIsInAir = false;
	bIsAccelerating = false;
	CharacterToAimRotation = FRotator::ZeroRotator;
	YawDelta = 0.f;
	TurnInPlace = 0.f;
	LeanScaling = 12.f;
	TurnInPlaceInterpSpeed = 30.f;
	PrevRotation = FRotator::ZeroRotator;
}

void UR4AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>( GetOwningActor() );
	if ( Owner.IsValid() )
		MovementComp = Owner->GetCharacterMovement();
}

void UR4AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Owner.IsValid() && MovementComp.IsValid())
	{
		// 땅에서 속도
		GroundSpeed = Owner->GetVelocity().Size2D();
		// Z축으로 속도
		ZSpeed = Owner->GetVelocity().Z;
		// 이동 방향 각도
		DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
		// 공중에 떠 있는지
		bIsInAir = MovementComp->IsFalling();
		// 가속중인지
		bIsAccelerating = MovementComp->GetCurrentAcceleration().Length() > 0.f;
		// Character와 Aim 사이 Rotation
		CharacterToAimRotation = ( Owner->GetBaseAimRotation() - Owner->GetActorRotation() ).GetNormalized();

		// delta
		FRotator delta = PrevRotation - Owner->GetActorRotation();
		delta.Normalize();
		
		// Turn in place
		TurnInPlace += delta.Yaw;
		TurnInPlace = FMath::FInterpTo( TurnInPlace, 0.f, DeltaSeconds, TurnInPlaceInterpSpeed );

		// Yaw Delta를 보간하여 계산
		float targetYaw = delta.Yaw / (LeanScaling * DeltaSeconds);
		YawDelta = FMath::FInterpTo(YawDelta, targetYaw, DeltaSeconds, 6.f);
		PrevRotation = Owner->GetActorRotation();
	}
}