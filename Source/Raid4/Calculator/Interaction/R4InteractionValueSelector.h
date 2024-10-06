#pragma once

#include "R4InteractValueCalculatorInterface.h"

#include "R4InteractionValueSelector.generated.h"

/**
 *  Value 값 설정 방식
 */
UENUM( BlueprintType )
enum class EInteractValueType : uint8
{
	Constant		UMETA( DisplayName = "상수" ),
	CustomClass		UMETA( DisplayName = "커스텀 클래스 사용" ),
};

/**
 *  Interaction 값 산출을 돕는 Value Selector.
 *  Value를 상수 또는 가해자 <-> 피해자 사이에 Custom Class로 계산한 값을 선택 가능
 */
USTRUCT( BlueprintType )
struct FR4InteractValueSelector
{
	GENERATED_BODY()

	FR4InteractValueSelector()
	: ValueType(EInteractValueType::Constant)
	, ValueCalculatorClass(nullptr)
	, Value(0.f)
	{}

	// 값을 return.
	FORCEINLINE float GetValue(const AActor* InInstigator, const AActor* InVictim) const
	{
		if(ValueType == EInteractValueType::CustomClass)
		{
			if(!IsValid(ValueCalculatorClass) ||
					!ensureMsgf(ValueCalculatorClass->ImplementsInterface(UR4InteractValueCalculatorInterface::StaticClass()),
					TEXT("Value Calculator class must implement IR4InteractValueCalculatorInterface.")))
				return Value;
			
			// CDO를 사용하여 가해자 <-> 피해자 사이의 특별한 Value를 산출
			const UObject* cdo = ValueCalculatorClass->GetDefaultObject(true);
			if(const IR4InteractValueCalculatorInterface* valueCalculator = Cast<IR4InteractValueCalculatorInterface>(cdo))
				return valueCalculator->CalculateInteractValue(InInstigator, InVictim);
		}

		return Value;
	}
	
	// 'Interaction Value'를 산출할 방식을 설정.
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EInteractValueType ValueType;

	// 'Interaction Value'를 산출 시 사용할 CustomClass. CDO를 사용.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=( EditCondition="ValueType == EInteractValueType::CustomClass", EditConditionHides, MustImplement = "/Script/Raid4.R4InteractValueCalculatorInterface" ))
	TSubclassOf<UObject> ValueCalculatorClass;

private:
	// Value. '상수'로 설정 시 이 값이 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(  AllowPrivateAccess = true, EditCondition="ValueType == EInteractValueType::Constant", EditConditionHides ))
	float Value;
};