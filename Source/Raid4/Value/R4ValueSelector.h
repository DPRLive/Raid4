#pragma once

#include "R4ValueCalculatorInterface.h"

#include "R4ValueSelector.generated.h"

/**
 *  Value 값 설정 방식
 */
UENUM( BlueprintType )
enum class EValueType : uint8
{
	Constant		UMETA( DisplayName = "상수" ),
	CustomClass		UMETA( DisplayName = "커스텀 클래스 사용" ),
};

/**
 *  값 산출을 돕는 Value Selector.
 *  Value를 상수 또는 가해자 <-> 피해자 사이에 Custom Class로 계산한 값을 선택 가능
 */
USTRUCT( BlueprintType )
struct FR4ValueSelector
{
	GENERATED_BODY()

	FR4ValueSelector()
	: ValueType(EValueType::Constant)
	, ValueCalculatorClass(nullptr)
	, Value(0.f)
	{}

	// 값을 return.
	FORCEINLINE float GetValue(const AActor* InInstigator, const AActor* InVictim) const
	{
		if(ValueType == EValueType::CustomClass)
		{
			if(!IsValid(ValueCalculatorClass) ||
					!ensureMsgf(ValueCalculatorClass->ImplementsInterface(UR4ValueCalculatorInterface::StaticClass()),
					TEXT("Value Calculator class must implement IR4ValueCalculatorInterface.")))
				return Value;
			
			// CDO를 사용하여 가해자 <-> 피해자 사이의 특별한 Value를 산출
			const UObject* cdo = ValueCalculatorClass->GetDefaultObject(true);
			if(const IR4ValueCalculatorInterface* valueCalculator = Cast<IR4ValueCalculatorInterface>(cdo))
				return valueCalculator->CalculateValue(InInstigator, InVictim);
		}

		return Value;
	}
	
	// 'Value'를 산출할 방식을 설정.
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EValueType ValueType;

	// 'Value'를 산출 시 사용할 CustomClass. CDO를 사용.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=( EditCondition="ValueType == EValueType::CustomClass", EditConditionHides, MustImplement = "/Script/Raid4.R4ValueCalculatorInterface" ))
	TSubclassOf<UObject> ValueCalculatorClass;

private:
	// Value. '상수'로 설정 시 이 값이 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(  AllowPrivateAccess = true, EditCondition="ValueType == EValueType::Constant", EditConditionHides ))
	float Value;
};