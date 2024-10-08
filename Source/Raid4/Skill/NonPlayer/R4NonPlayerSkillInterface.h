// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4NonPlayerSkillInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4NonPlayerSkillInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Non Player Skill을 위한 interface.
 */
class RAID4_API IR4NonPlayerSkillInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// AI 스킬을 사용
	virtual void ActivateAISkill() = 0;

	// Target과의 Skill 사용 가능 최소 범위를 return
	virtual float GetActivateSkillMinDist() const = 0;
};
