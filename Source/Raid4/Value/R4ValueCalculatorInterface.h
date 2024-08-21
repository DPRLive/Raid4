// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4ValueCalculatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4ValueCalculatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAID4_API IR4ValueCalculatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  가해자 <-> 피해자 사이에 Value로 사용할 값을 계산하여 return.
	*  CDO를 사용함!
	*  @param InInstigator : 가해자 객체
	*  @param InVictim : 피해자 객체
	*/
	virtual float CalculateValue(const AActor* InInstigator, const AActor* InVictim) const = 0;
};
