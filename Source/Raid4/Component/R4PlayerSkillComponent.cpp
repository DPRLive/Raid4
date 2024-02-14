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
void UR4PlayerSkillComponent::OnInputSkillStarted(ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(SkillInstancePtrs.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputSkillStarted();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillTriggered(ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(SkillInstancePtrs.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputSkillTriggered();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillCompleted(ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(SkillInstancePtrs.IsValidIndex(realIdx))
	{
		if(IR4PlayerSkillInterface* playerSkill = Cast<IR4PlayerSkillInterface>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputSkillCompleted();
		}
	}
}
