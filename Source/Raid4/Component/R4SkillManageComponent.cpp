// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillManageComponent)

UR4SkillManageComponent::UR4SkillManageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

/**
 *	컴포넌트 초기화
 */
void UR4SkillManageComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

/**
 *  begin play
 */
void UR4SkillManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// for(auto& elem : Skills)
	// {
	// 	InstancedSkills.Emplace(NewObject<UR4SkillBase>(this, elem));
	// 	InstancedSkills.Last()->RegisterComponent();
	// 	
	// 	if(InstancedSkills.Last()->GetOwnerRole() == ROLE_None)
	// 	{
	// 		LOG_WARN(LogTemp, TEXT("No role."));
	// 	}
	// 	else if(InstancedSkills.Last()->GetOwnerRole() == ROLE_Authority)
	// 	{
	// 		LOG_WARN(LogTemp, TEXT("Auth."));
	// 	}
	// 	else if(InstancedSkills.Last()->GetOwnerRole() == ROLE_AutonomousProxy)
	// 	{
	// 		LOG_WARN(LogTemp, TEXT("Auto."));
	// 	}
	// }
}
