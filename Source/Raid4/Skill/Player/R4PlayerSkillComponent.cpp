// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillComponent.h"
#include "../../Input/R4PlayerInputableInterface.h"
#include "../R4SkillBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerSkillComponent)

UR4PlayerSkillComponent::UR4PlayerSkillComponent()
{
}

/**
 *  입력 연결
 */
void UR4PlayerSkillComponent::OnInputSkillStarted( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstancePtrs.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstancePtrs[realIdx] ) )
			playerSkill->OnInputStarted();
	}
}

void UR4PlayerSkillComponent::OnInputSkillTriggered( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstancePtrs.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstancePtrs[realIdx] ) )
			playerSkill->OnInputTriggered();
	}
}

void UR4PlayerSkillComponent::OnInputSkillCompleted( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstancePtrs.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstancePtrs[realIdx] ) )
			playerSkill->OnInputCompleted();
	}
}
