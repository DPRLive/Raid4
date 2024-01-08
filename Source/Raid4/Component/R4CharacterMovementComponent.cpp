// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterMovementComponent)

UR4CharacterMovementComponent::UR4CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Simple move to location에서 Character Movement Comp의 Replicate System을 이용하기 위해 가속 기반 path following 사용
	// UPathFollowingComponent::FollowPathSegment 에서 bUseAccelerationForPaths가 true일 시 AddMovement로 가속하는걸 확인
	bUseAccelerationForPaths = true;
	bWantsInitializeComponent = true;

	AccelerationTime = 0.01f;
}

/**
 *	컴포넌트 초기화.
 *	Replicate을 위해 가속 기반 움직임을 사용하면서, 즉시 가속 및 감속이 가능하도록 최대 가속 & 감속 걷기 제동 설정
 */
void UR4CharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// AccelerationTime 만에 가속 / 감속 하기 위한 계산
	MaxAcceleration = MaxWalkSpeed / AccelerationTime;
	BrakingDecelerationWalking = MaxWalkSpeed / AccelerationTime;
}