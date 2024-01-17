// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerSkillInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4PlayerSkillInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어 스킬 정의 시, 입력 처리를 위한 인터페이스 
 */
class RAID4_API IR4PlayerSkillInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 스킬 입력을 받았다!
	virtual void OnInputSkillStarted() = 0;
	virtual void OnInputSkillTriggered() = 0;
	virtual void OnInputSkillCompleted() = 0;
};
