// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4SkillComponent.h"
#include "R4PlayerSkillComponent.generated.h"

/**
 * Player용 Skill Component
 */
UCLASS()
class RAID4_API UR4PlayerSkillComponent : public UR4SkillComponent
{
	GENERATED_BODY()

public:
	UR4PlayerSkillComponent();

	// 스킬 입력 처리
	virtual void OnInputSkillStarted(ESkillIndex InSkillIndex);
	virtual void OnInputSkillTriggered(ESkillIndex InSkillIndex);
	virtual void OnInputSkillCompleted(ESkillIndex InSkillIndex);
};
