#include "UtilDamage.h"

#include "../Stat/R4TagStatQueryInterface.h"
#include "../Stat/R4StatStruct.h"

/**
*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출.
*  '가해자 입장'에서만 데미지를 산출.
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InDamageDesc : 데미지에 관한 정보.
*/
FR4DamageReceiveInfo UtilDamage::CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc)
{
	FR4DamageReceiveInfo retDamageInfo;

	// 데미지 계산
	retDamageInfo.IncomingDamage = InDamageDesc.Value.GetValue(InInstigator, InVictim);
		
	// 고정 데미지가 아닐시 데미지 증감 계산
	if (!InDamageDesc.bFixedDamage)
	{
		float i_CriticalChance = 0.f, i_ApplyDamageMultiplier = 1.f;
		
		if(IsValid(InInstigator))
		{
			if(const IR4TagStatQueryInterface* instigator = Cast<IR4TagStatQueryInterface>(InInstigator))
			{
				// 크리티컬 확률
				if(FR4StatInfo* criticalStat = instigator->GetStatByTag(TAG_STAT_NORMAL_CriticalChance))
					i_CriticalChance = criticalStat->GetTotalValue();

				// 데미지 증감량
				if(FR4StatInfo* applyDamageMultiplier = instigator->GetStatByTag(TAG_STAT_NORMAL_ApplyDamageMultiplier))
					i_ApplyDamageMultiplier = applyDamageMultiplier->GetTotalValue();
			}
		}
		
		// 랜덤 변동성 추가
		retDamageInfo.IncomingDamage *= GetRandomFactor();
		
		// 치명타 계산
		retDamageInfo.bCritical = DetermineCritical(i_CriticalChance, retDamageInfo.IncomingDamage);

		// 가해자의 가해 데미지 증감량 곱연산
		retDamageInfo.IncomingDamage *= i_ApplyDamageMultiplier;
	}
	
	return retDamageInfo;
}

/**
*  R4DamageApplyDesc들을 기반으로 데미지를 합산하여 R4DamageReceiveInfo를 산출
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InDamageDescs : 데미지에 관한 정보들
*/
FR4DamageReceiveInfo UtilDamage::CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const TArray<FR4DamageApplyDesc>& InDamageDescs)
{
	FR4DamageReceiveInfo totalDamageInfo;

	for(auto& damageDesc : InDamageDescs)
	{
		FR4DamageReceiveInfo tempInfo = CalculateDamageReceiveInfo(InInstigator, InVictim, damageDesc);

		// 계산된 데미지 합산
		totalDamageInfo.IncomingDamage += tempInfo.IncomingDamage;

		// 하나라도 크리티컬이면 크리티컬로 판정
		totalDamageInfo.bCritical |= tempInfo.bCritical;
	}
	
	return totalDamageInfo;
}

/**
*  방어력을 기준으로 방어 감소율을 계산.
*/
float UtilDamage::CalculateReductionByArmor(float InArmor)
{
	float damageReduction = InArmor / (InArmor + Damage::G_DefenseConstant);
	return (1.0f - damageReduction);
}

/**
*  랜덤 데미지 변동율 리턴.
*/
float UtilDamage::GetRandomFactor()
{
	return FMath::RandRange(Damage::G_RandomFactorLower, Damage::G_RandomFactorUpper);
}

/**
*  Critical 판정
*  @param InCriticalChance : 0 ~ 100. %
*  @param OutDamage : 크리티컬 여부에 따라 변동 시킬 Damage
*  @return : 크리티컬 여부
*/
bool UtilDamage::DetermineCritical(float InCriticalChance, float& OutDamage)
{
	if(bool bCritical = FMath::RandRange(0.0f, 1.0f) <= (InCriticalChance / 100.f))
	{
		OutDamage *= Damage::G_CriticalFactor;
		return true;
	}
	
	return false;
}
