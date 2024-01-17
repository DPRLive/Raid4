// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "../Component/R4PlayerInputComponent.h"
#include "../Component/R4CameraManageComponent.h"
#include "../Skill/R4SkillBase.h"
#include "../Interface/R4PlayerSkillInterface.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerCharacter)

/**
 *  생성자
 */
APlayerCharacter::APlayerCharacter(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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
	TObjectPtr<UR4SkillBase>* skill = InstancedSkills.Find(InSkillIndex);
	if(skill == nullptr)
	{
		LOG_WARN( R4Data, TEXT("SkillIndex : [%s] is nullptr."), *ENUM_TO_STRING( ESkillIndex, InSkillIndex ) );
		return;
	}

	// TODO : Manage Comp에 허락 받고 입력 넣기
	if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(*skill))
	{
		playerSkill->OnInputSkillStarted();
	}
}

/**
 *  스킬 입력 처리
 */
void APlayerCharacter::OnInputSkillTriggered(const ESkillIndex InSkillIndex)
{
	TObjectPtr<UR4SkillBase>* skill = InstancedSkills.Find(InSkillIndex);
	if(skill == nullptr)
	{
		LOG_WARN( R4Data, TEXT("SkillIndex : [%s] is nullptr."), *ENUM_TO_STRING( ESkillIndex, InSkillIndex ) );
		return;
	}
	
	if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(*skill))
	{
		playerSkill->OnInputSkillTriggered();
	}
}

/**
 *  스킬 입력 종료 처리
 */
void APlayerCharacter::OnInputSkillCompleted(const ESkillIndex InSkillIndex)
{
	TObjectPtr<UR4SkillBase>* skill = InstancedSkills.Find(InSkillIndex);
	if(skill == nullptr)
	{
		LOG_WARN( R4Data, TEXT("SkillIndex : [%s] is nullptr."), *ENUM_TO_STRING( ESkillIndex, InSkillIndex ) );
		return;
	}
	
	if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(*skill))
	{
		playerSkill->OnInputSkillCompleted();
	}
}
