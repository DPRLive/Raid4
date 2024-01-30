// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillComponent.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillComponent)

UR4SkillComponent::UR4SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

/**
 *	컴포넌트 초기화
 */
void UR4SkillComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UR4SkillComponent, InstancedSkills, COND_OwnerOnly);
}

/**
 *  begin play
 */
void UR4SkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
 *  스킬을 추가한다. (서버)
 */
void UR4SkillComponent::Server_AddSkill(const ESkillIndex InSkillIndex, UR4SkillBase* InSkill)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);

	// 음 인덱스가 부족한걸? -> resize
	if(realIdx >= InstancedSkills.Num())
	{
		InstancedSkills.SetNum(realIdx + 1);
	}

	InstancedSkills[realIdx] = InSkill;
}
