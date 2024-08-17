// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4BuffValueCalculatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4BuffValueCalculatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAID4_API IR4BuffValueCalculatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  Buff Value로 사용할 값을 계산하여 return.
	*  @param InInstigator : 버프 시전 객체
	*  @param InVictim : 버프 받는 객체
	*/
	virtual float CalculateBuffValue(const AActor* InInstigator, const AActor* InVictim) const = 0;
};
