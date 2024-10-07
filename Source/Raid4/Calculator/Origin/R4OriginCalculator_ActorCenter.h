// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Object.h>

#include "R4OriginCalculatorInterface.h"

#include "R4OriginCalculator_ActorCenter.generated.h"

/**
 * Actor의 Center를 Origin으로 계산.
 */
UCLASS( NotBlueprintable, ClassGroup=(Calculator) )
class RAID4_API UR4OriginCalculator_ActorCenter : public UObject, public IR4OriginCalculatorInterface
{
	GENERATED_BODY()

public:
	// Actor의 Center를 Origin으로 계산.
	virtual FTransform CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const override;
};
