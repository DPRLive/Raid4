// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4ServePlayerSkillInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4ServePlayerSkillInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어 스킬을 제공하는 객체에 붙이는 interface
 */
class RAID4_API IR4ServePlayerSkillInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 스킬 입력 처리가 가능해야함.
	virtual void OnInputSkillStarted(const ESkillIndex InSkillIndex) = 0;
	virtual void OnInputSkillTriggered(const ESkillIndex InSkillIndex) = 0;
	virtual void OnInputSkillCompleted(const ESkillIndex InSkillIndex) = 0;
};
