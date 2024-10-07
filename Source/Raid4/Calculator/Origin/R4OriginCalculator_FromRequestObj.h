// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4OriginCalculatorInterface.h"

#include <UObject/Object.h>

#include "R4OriginCalculator_FromRequestObj.generated.h"

/**
 * RequestObj에게 Origin을 제공받는 클래스.
 * Request Obj가 IR4OriginProviderInterface를 통해 Origin을 제공할 수 있어야함!
 */
UCLASS( NotBlueprintable, ClassGroup=(Calculator) )
class RAID4_API UR4OriginCalculator_FromRequestObj : public UObject, public IR4OriginCalculatorInterface
{
	GENERATED_BODY()

public:
	// Actor의 Center를 Origin으로 계산.
	virtual FTransform CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const override;
};
