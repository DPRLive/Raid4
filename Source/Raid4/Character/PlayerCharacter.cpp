// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../Component/R4PlayerInputComponent.h"
#include "../Component/R4CameraManageComponent.h"
#include "../Component/R4PlayerSkillComponent.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerCharacter)

/**
 *  생성자. Skill Comp를 Player용으로 변경. 찾아보니 SetDefaultSubobjectClass를 여러번 .으로 이어서 쓸 수 있는듯 하다
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
 *  스킬 입력 시작 처리
 */
void APlayerCharacter::OnInputSkillStarted(const ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* skillComp = Cast<IR4PlayerSkillInputInterface>(SkillComp))
	{
		skillComp->OnInputSkillStarted(InSkillIndex);
	}
}

/**
 *  스킬 입력 처리
 */
void APlayerCharacter::OnInputSkillTriggered(const ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* skillComp = Cast<IR4PlayerSkillInputInterface>(SkillComp))
	{
		skillComp->OnInputSkillTriggered(InSkillIndex);
	}
}

/**
 *  스킬 입력 종료 처리
 */
void APlayerCharacter::OnInputSkillCompleted(const ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* skillComp = Cast<IR4PlayerSkillInputInterface>(SkillComp))
	{
		skillComp->OnInputSkillCompleted(InSkillIndex);
	}
}
