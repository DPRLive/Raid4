#pragma once

#include "R4BuffModifyDesc.generated.h"

class UR4BuffBase;

/**
 * 런타임에 Buff의 값을 변경해서 적용할 때 사용할 Desc
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4BuffModifyDesc
{
	GENERATED_BODY()

	FR4BuffModifyDesc()
		: TimeFactor(1.f)
		, ValueFactor(1.f)
		{}
	
	// 시간 값에 곱하고 싶은 factor, 버프에 따라 다르게 동작할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	float TimeFactor;

	// 버프의 '값'에 곱하고 싶은 Factor, 버프에 따라 다르게 동작할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	float ValueFactor;
};
