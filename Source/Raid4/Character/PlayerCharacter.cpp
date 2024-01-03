// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../Input/R4PlayerInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerCharacter)

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// InputComponent
	PlayerInputComp = CreateDefaultSubobject<UR4PlayerInputComponent>(TEXT("PlayerInputComp"));
}

/**
 *  begin play
 */
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  SetupPlayerInputComponent
 */
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(OnSetupPlayerInput.IsBound())
		OnSetupPlayerInput.Broadcast(PlayerInputComponent);
}