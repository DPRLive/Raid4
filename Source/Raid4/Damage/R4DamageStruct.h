#pragma once

#include "../Calculator/Interaction/R4InteractionValueSelector.h"
#include "R4DamageStruct.generated.h"

/**
 * 가할 데미지의 명세를 담는 Damage Desc
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4DamageApplyDesc
{
	GENERATED_BODY()

	FR4DamageApplyDesc()
	: Value(FR4InteractValueSelector())
	, bFixedDamage(false)
	{ }

	// 사용할 Value. 
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	FR4InteractValueSelector Value;
	
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