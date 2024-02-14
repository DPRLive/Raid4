// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerSkillInputInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4PlayerSkillInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어의 스킬 입력을 받아야 할 때 사용
 */
class RAID4_API IR4PlayerSkillInputInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 스킬 입력 처리가 가능해야함.
	virtual void OnInputSkillStarted(ESkillIndex InSkillIndex) = 0;
	virtual void OnInputSkillTriggered(ESkillIndex InSkillIndex) = 0;
	virtual void OnInputSkillCompleted(ESkillIndex InSkillIndex) = 0;
};
