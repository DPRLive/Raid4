// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4Damageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Damage를 입을 수 있는 (공격 당할 수 있는) 객체를 위한 인터페이스
 */
class RAID4_API IR4Damageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ReceiveDamage(AActor* InInstigator, float InDamage) = 0;
};
