// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillComponent.h"
#include "../Interface/R4PlayerSkillInputInterface.h"
#include "R4PlayerSkillComponent.generated.h"

/**
 * Player용 Skill Component
 */
UCLASS()
class RAID4_API UR4PlayerSkillComponent : public UR4SkillComponent, public IR4PlayerSkillInputInterface
{
	GENERATED_BODY()

public:
	UR4PlayerSkillComponent();

	// 스킬 입력 처리
	virtual void OnInputSkillStarted(const ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillTriggered(const ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillCompleted(const ESkillIndex InSkillIndex) override;
};
