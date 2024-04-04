// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../Component/R4PlayerInputComponent.h"
#include "../Component/R4CameraManageComponent.h"
#include "../Component/R4PlayerSkillComponent.h"
#include "../Component/R4CharacterMovementComponent.h"
#include "../Data/DataAsset/R4DAPCCommonData.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerCharacter)

/**
 *  생성자. Skill Comp를 Player용으로 변경. 찾아보니 SetDefaultSubobjectClass를 여러번 .으로 이어서 쓸 수 있다
 */
APlayerCharacter::APlayerCharacter(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4PlayerSkillComponent>(FName(TEXT("SkillComp"))))
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

	_InitPlayerCharacterCommonData();
}

/**
 *  begin play
 */
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  SetupPlayerInputComponent, Player Controller가 Pose 할 때 호출
 */
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InPlayerInputComponent);

	if(OnSetupPlayerInput.IsBound())
		OnSetupPlayerInput.Broadcast(InPlayerInputComponent);
	
}

/**
 *  PlayerController를 리턴
 */
APlayerController* APlayerCharacter::GetPlayerController()
{
	return Cast<APlayerController>(GetController());
}

/**
 *  이동 멈춤
 */
void APlayerCharacter::StopMove()
{
	if(UPawnMovementComponent* moveComp = GetMovementComponent())
	{
		moveComp->StopMovementImmediately();
	}
}

/**
 *  이동 입력
 */
void APlayerCharacter::AddMovement(const FVector& InWorldDir)
{
	AddMovementInput(InWorldDir);
}

/**
 *  특정 위치로 이동
 */
void APlayerCharacter::MoveToLocation(const FVector& InLoc)
{
	if(UR4CharacterMovementComponent* moveComp = Cast<UR4CharacterMovementComponent>(GetMovementComponent()))
	{
		moveComp->MoveToLocation(GetController(), InLoc);
	}
}

/**
 *  스킬 입력 시작 처리
 */
void APlayerCharacter::OnInputSkillStarted(ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* skillComp = Cast<IR4PlayerSkillInputable>(SkillComp))
	{
		skillComp->OnInputSkillStarted(InSkillIndex);
	}
}

/**
 *  스킬 입력 처리
 */
void APlayerCharacter::OnInputSkillTriggered(ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* skillComp = Cast<IR4PlayerSkillInputable>(SkillComp))
	{
		skillComp->OnInputSkillTriggered(InSkillIndex);
	}
}

/**
 *  스킬 입력 종료 처리
 */
void APlayerCharacter::OnInputSkillCompleted(ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* skillComp = Cast<IR4PlayerSkillInputable>(SkillComp))
	{
		skillComp->OnInputSkillCompleted(InSkillIndex);
	}
}

/**
 *  PlayerCharacter들의 공통된 데이터를 초기화한다.  
 */
void APlayerCharacter::_InitPlayerCharacterCommonData()
{
	if(PlayerCharacterCommonData == nullptr)
	{
		LOG_ERROR(R4Data, TEXT("PlayerCharacterCommonData is nullptr."));
		return;
	}

	bUseControllerRotationYaw = PlayerCharacterCommonData->bUseControllerRotationYaw;
	
	if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
	{
		moveComp->bOrientRotationToMovement = PlayerCharacterCommonData->bOrientRotationToMovement;
		moveComp->bUseControllerDesiredRotation = PlayerCharacterCommonData->bUseControllerDesiredRotation;
		moveComp->RotationRate = PlayerCharacterCommonData->RotationRate;
	}

	SpringArmComp->TargetArmLength = PlayerCharacterCommonData->TargetArmLength;
	SpringArmComp->SetRelativeRotation(PlayerCharacterCommonData->RelativeRotation);
	SpringArmComp->bUsePawnControlRotation = PlayerCharacterCommonData->bUsePawnControlRotation;
	SpringArmComp->bInheritPitch = PlayerCharacterCommonData->bInheritPitch;
	SpringArmComp->bInheritYaw = PlayerCharacterCommonData->bInheritYaw;
	SpringArmComp->bInheritRoll = PlayerCharacterCommonData->bInheritRoll;
	SpringArmComp->bDoCollisionTest = PlayerCharacterCommonData->bDoCollisionTest;
}
