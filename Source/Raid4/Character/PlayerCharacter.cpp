// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../Component/R4PlayerInputComponent.h"
#include "../Component/R4CameraManageComponent.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerCharacter)

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Input Component
	PlayerInputComp = CreateDefaultSubobject<UR4PlayerInputComponent>(TEXT("PlayerInputComp"));

	// SpringArm Comp
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
		
	// Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	// Camera Manage Comp
	CameraManageComp = CreateDefaultSubobject<UR4CameraManageComponent>(TEXT("CameraManageComp"));
}

/**
 *  Post Init
 */
void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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