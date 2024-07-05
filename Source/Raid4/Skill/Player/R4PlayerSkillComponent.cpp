// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillComponent.h"
#include "../../Input/R4PlayerInputable.h"

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
		if(IR4PlayerInputable* playerSkill = Cast<IR4PlayerInputable>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputStarted();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillTriggered(ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(SkillInstancePtrs.IsValidIndex(realIdx))
	{
		if(IR4PlayerInputable* playerSkill = Cast<IR4PlayerInputable>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputTriggered();
		}
	}
}

void UR4PlayerSkillComponent::OnInputSkillCompleted(ESkillIndex InSkillIndex)
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if(SkillInstancePtrs.IsValidIndex(realIdx))
	{
		if(IR4PlayerInputable* playerSkill = Cast<IR4PlayerInputable>(SkillInstancePtrs[realIdx]))
		{
			playerSkill->OnInputCompleted();
		}
	}
}
