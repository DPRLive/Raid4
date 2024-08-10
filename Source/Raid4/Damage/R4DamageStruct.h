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
	: ExpressionClass(nullptr)
	, Value(0.f)
	, bFixedDamage(false)
	{ }

	// 사용할 Expression Class. CDO를 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta = (MustImplement = "R4BaseDamageExpressionInterface") )
	TSubclassOf<UObject> ExpressionClass;
	
	// Value. Expression Class에 따라 다르게 Value가 사용될 수 있음.
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
	{ }

	// 계산된 데미지 (증감이 모두 적용 된 상태)
	UPROPERTY( VisibleInstanceOnly, Transient )
	float IncomingDamage;

	// 크리티컬 인지
	UPROPERTY( VisibleInstanceOnly, Transient )
	uint8 bCritical:1;
};