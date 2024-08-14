#pragma once

#include "../Damage/R4DamageStruct.h"

namespace UtilDamage
{
	// R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출, '가해자 입장'에서만 데미지를 산출.
	FR4DamageReceiveInfo CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc);

	// R4DamageApplyDesc들을 기반으로 데미지를 합산하여 R4DamageReceiveInfo를 산출, '가해자 입장'에서만 데미지를 산출.
	FR4DamageReceiveInfo CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const TArray<FR4DamageApplyDesc>& InDamageDescs);

	// 방어력을 기준으로 방어 감소율을 계산.
	float CalculateReductionByArmor(float InArmor);

	// 랜덤 데미지 변동율 리턴.
	float GetRandomFactor();

	// Critical 판정
	bool DetermineCritical(float InCriticalChance, float& OutDamage);
}
