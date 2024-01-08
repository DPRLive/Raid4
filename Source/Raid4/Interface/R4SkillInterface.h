// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4SkillInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4SkillInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 객체가 R4SkillManageComponent를 들고 있을 시 사용
 */
class RAID4_API IR4SkillInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 객체가 스킬을 사용할 수 있는지 확인할 때 사용할 함수
	virtual bool CanActivateSkill() = 0;
};
