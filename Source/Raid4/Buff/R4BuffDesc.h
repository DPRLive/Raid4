#pragma once

#include "R4BuffDesc.generated.h"

/**
 *  버프 적용 방식
 */
UENUM( BlueprintType )
enum class EBuffMode : uint8
{
	Instant			UMETA( DisplayName = "즉시 적용" ),
	Interval		UMETA( DisplayName = "일정 시간마다 적용" ),
};

/**
 *  버프 지속 시간 방식
 */
UENUM( BlueprintType )
enum class EBuffDurationType : uint8
{
	OneShot			UMETA( DisplayName = "일회성" ),
	Duration		UMETA( DisplayName = "지속 시간" ),
	Infinite		UMETA( DisplayName = "무한" ),
};

/**
 *  버프 Value 값 설정 방식
 */
UENUM( BlueprintType )
enum class EBuffValueType : uint8
{
	Constant		UMETA( DisplayName = "상수" ),
	CustomClass		UMETA( DisplayName = "커스텀 클래스 사용" ),
};

/**
 * Buff 사용 시 필요한 인수들
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4BuffDesc
{
	GENERATED_BODY()

	FR4BuffDesc()
		: BuffMode(EBuffMode::Instant)
		, IntervalTime(0.f)
		, BuffDurationType(EBuffDurationType::OneShot)
		, Duration(0.f)
		, BuffValueType(EBuffValueType::Constant)
		, Value(0.f)
		{ }

	// 버프 적용 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffMode BuffMode;
	
	// 버프가 간격을 두고 실행이 될때, 간격의 시간을 설정
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuffMode == EBuffMode::Interval", EditConditionHides, ClampMin = "0.0", UIMin = "0.0"))
	float IntervalTime;
	
	// 버프 지속 시간 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffDurationType BuffDurationType;
	
	// 버프의 '지속 시간'으로 사용, 버프에 따라 다르게 동작할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuffDurationType == EBuffDurationType::Duration", EditConditionHides, ClampMin = "0.0", UIMin = "0.0"))
	float Duration;

	// 버프의 'Value'를 산출할 방식을 설정.
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffValueType BuffValueType;

	// 버프의 'Value'를 산출 시 사용할 CustomClass. CDO를 사용.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuffValueType == EBuffValueType::CustomClass", EditConditionHides, MustImplement = "R4BuffValueCalculatorInterface"))
	TSubclassOf<UObject> BuffValueCalculatorClass;
	
	// 버프의 'Value'으로 사용, 버프에 따라 다르게 Value를 사용할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta=(EditCondition="BuffValueType == EBuffValueType::Constant", EditConditionHides))
	float Value;
};
