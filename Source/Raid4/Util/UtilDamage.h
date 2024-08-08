#pragma once

#include "../Damage/R4DamageStruct.h"

namespace UtilDamage
{
	/**
	*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출
	*  @param InInstigator : 데미지를 가하는(Apply) 객체
	*  @param InVictim : 데미지를 입는(Receive) 객체
	*  @param InDamageDesc : 데미지에 관한 정보.
	*/
	FR4DamageReceiveInfo CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc);
}
