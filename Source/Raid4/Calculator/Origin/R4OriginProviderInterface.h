// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4OriginProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4OriginProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Origin Calculator에게 Origin을 제공할 수 있는 클래스를 위한 interface.
 */
class RAID4_API IR4OriginProviderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	/**
	*  특정 Origin 값을 제공
	*  @param InRequestObj : 계산을 요청한 Object
	*  @param InActor : 계산의 기준이 되는 Actor.
	*/
	virtual FTransform GetOrigin( const UObject* InRequestObj, const AActor* InActor ) const = 0;
};
