// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterMovementComponent)

UR4CharacterMovementComponent::UR4CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}

/**
 *	컴포넌트 초기화.
 */
void UR4CharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
