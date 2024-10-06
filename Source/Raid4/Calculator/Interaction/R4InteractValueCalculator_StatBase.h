// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4InteractValueCalculatorInterface.h"

#include <UObject/Object.h>

#include "R4InteractValueCalculator_StatBase.generated.h"

/**
 * Stat에 기반하여 Value를 구하는 Calculator Class.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Calculator) )
class RAID4_API UR4InteractValueCalculator_StatBase : public UObject, public IR4InteractValueCalculatorInterface
{
	GENERATED_BODY()
	
public:
	UR4InteractValueCalculator_StatBase();
	
	/**
	*  Stat에 기반한 Interact Value를 계산.
	*  @param InInstigator : 가해자 객체
	*  @param InVictim : 피해자 객체
	*/
	virtual float CalculateInteractValue( const AActor* InInstigator, const AActor* InVictim ) const override;

private:
	// 상대의 스탯인지 나의 스탯인지
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ETargetType TargetStatOwner;
	
	// 기반이 되는 스탯
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat", AllowPrivateAccess = true))
	FGameplayTag StatTag;

	// 스탯 피연산자 타입
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EStatOperandType OperandType;

	// 비율 ( 해당 Stat 값에 곱해져서 Value가 구해짐 )
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Proportion;
};
