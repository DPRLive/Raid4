// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4OriginCalculatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4OriginCalculatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 특정 Actor의 Origin 계산을 돕는 Strategy Class Interface.
 */
class RAID4_API IR4OriginCalculatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  특정 Origin 값을 계산하여 return.
	*  CDO를 사용!
	*  @param InRequestObj : 계산을 요청한 Object
	*  @param InActor : 계산의 기준이 되는 Actor.
	*/
	virtual FTransform CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const = 0;
};
