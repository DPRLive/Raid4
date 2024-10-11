// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4OriginCalculatorInterface.h"

#include <UObject/Object.h>

#include "R4OriginCalculator_PointToPoint.generated.h"

/**
 * 두개의 Calculator를 통해서, A -> B를 바라보는 A 기준 Origin을 구하는 Calculator.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Calculator) )
class RAID4_API UR4OriginCalculator_PointToPoint : public UObject, public IR4OriginCalculatorInterface
{
	GENERATED_BODY()

public:
	// Mesh의 Socket을 기준으로 Origin으로 계산.
	virtual FTransform CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const override;

private:
	FTransform _CalculateOriginByCDO( const TSubclassOf<UObject>& InOriginCalculator, const UObject* InRequestObj, const AActor* InActor ) const;
	
private:
	// A 지점의 기준이 되는 Origin을 구하는 Calculator
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4OriginCalculatorInterface" ) )
	TSubclassOf<UObject> AOriginCalculator;

	// B지점의 기준이 되는 Origin을 구하는 Calculator
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4OriginCalculatorInterface" ) )
	TSubclassOf<UObject> BOriginCalculator;
};
