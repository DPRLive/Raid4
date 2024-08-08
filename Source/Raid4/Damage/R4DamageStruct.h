#pragma once

#include "R4DamageStruct.generated.h"

/**
 * 가할 데미지의 명세를 담는 Damage Desc
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4DamageApplyDesc
{
	GENERATED_BODY()

	FR4DamageApplyDesc()
	: CalculatorClass(nullptr)
	, Value(0.f)
	, bFixedDamage(false)
	{ }

	// 사용할 Calculator Class. CDO를 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta = (MustImplement = "R4DamageCalculatorInterface") )
	TSubclassOf<UObject> CalculatorClass;
	
	// Value. Calculator Class에 따라 다르게 Value가 사용될 수 있음.
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	float Value;

	// 고정 데미지 (데미지 증감의 영향을 받지 않는지)
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	uint8 bFixedDamage:1;
};

/**
 * 받을 데미지의 정보를 담는 Damage Info
 * TODO : 압축하면 좋을 드드듯
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4DamageReceiveInfo
{
	GENERATED_BODY()

	FR4DamageReceiveInfo()
	: IncomingDamage(0.f)
	, bCritical(false)
	, bFixedDamage(false)
	{ }

	// 가해자가 계산한 데미지
	UPROPERTY( VisibleInstanceOnly, Transient )
	float IncomingDamage;

	// 크리티컬 인지
	UPROPERTY( VisibleInstanceOnly, Transient )
	uint8 bCritical:1;
	
	// 고정 데미지인지 (데미지 증감의 영향을 받지 않는지)
	UPROPERTY( VisibleInstanceOnly, Transient )
	uint8 bFixedDamage:1;
};