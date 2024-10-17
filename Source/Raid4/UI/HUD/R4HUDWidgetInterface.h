// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4HUDWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE()
class UR4HUDWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  HUD를 위한 Interface
 */
class RAID4_API IR4HUDWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Skill SetUp을 진행
	virtual void SetupSkillWidget( UUserWidget* InWidget ) = 0;

	// Skill Cooldown을 반환
	virtual float GetSkillCooldownRemaining( int32 InSkillIndex ) = 0;
};
