// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillComponent.h"
#include "../Interface/R4PlayerSkillInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerSkillComponent)

UR4PlayerSkillComponent::UR4PlayerSkillComponent()
{
}

/**
 *  입력 연결
 */
void UR4PlayerSkillComponent::OnInputSkillStarted(const ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(InstancedSkills.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(InstancedSkills[realIdx]))
		{
			playerSkill->OnInputSkillStarted();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillTriggered(const ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(InstancedSkills.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(InstancedSkills[realIdx]))
		{
			playerSkill->OnInputSkillTriggered();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillCompleted(const ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(InstancedSkills.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(InstancedSkills[realIdx]))
		{
			playerSkill->OnInputSkillCompleted();
		}
	}
}
