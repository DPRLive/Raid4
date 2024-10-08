// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4CharacterAIInterface.generated.h"

class AController;

// This class does not need to be modified.
UINTERFACE()
class UR4CharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * AI Character를 위한 Interface.
 */
class RAID4_API IR4CharacterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 해당 AI에게 데미지를 입힌 Controller TMap을 반환.
	virtual const TMap<TWeakObjectPtr<const AController>, float>& GetDamagedControllers() const = 0;

	// Patrol Radius 범위를 반환.
	virtual float GetMaxPatrolRadius() const = 0;
};
