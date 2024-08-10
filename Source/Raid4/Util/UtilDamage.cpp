#include "UtilDamage.h"

#include "../Damage/Expression/R4BaseDamageExpressionInterface.h"
#include "../Stat/R4StatBaseComponent.h"

/**
*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InDamageDesc : 데미지에 관한 정보.
*/
FR4DamageReceiveInfo UtilDamage::CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc)
{
	FR4DamageReceiveInfo retDamageInfo;
	
	if(!IsValid(InDamageDesc.ExpressionClass) ||
		!ensureMsgf(InDamageDesc.ExpressionClass->ImplementsInterface(UR4BaseDamageExpressionInterface::StaticClass()),
		TEXT("Damage Expression class must implement IR4BaseDamageExpressionInterface.")))
	{
		LOG_ERROR(R4Data, TEXT("Base Damage Calculate Failed. Expression Class is invalid."))
		return retDamageInfo;
	}
	
	// Expression class CDO를 이용하여 증감이 안된 순수한 데미지 계산.
	{
		const UObject* cdo = InDamageDesc.ExpressionClass->GetDefaultObject(true);
		if(const IR4BaseDamageExpressionInterface* damageCalculator = Cast<IR4BaseDamageExpressionInterface>(cdo))
			retDamageInfo.IncomingDamage = damageCalculator->CalculateBaseDamage(InInstigator, InVictim, InDamageDesc.Value);
	}

	// 고정 데미지가 아닐시 데미지 증감 계산
	if (!InDamageDesc.bFixedDamage)
	{
		// 데미지 증감 계산에 필요한 변수들.
		// Instigator와 Victim의 StatComp에서 찾기 i : instigator, v : victim
		float i_CriticalChance = 0.f, v_Armor = 0.f;
		
		if(IsValid(InInstigator))
		{
			if(UR4StatBaseComponent* instigatorStat = InInstigator->FindComponentByClass<UR4StatBaseComponent>())
			{
				if(FR4StatData* criticalStat = instigatorStat->GetStatByTag<FR4StatData>(TAG_STAT_NORMAL_CriticalChance))
					i_CriticalChance = criticalStat->GetBaseValue() + criticalStat->GetModifierValue();
			}
		}
		
		if(IsValid(InVictim))
		{
			if(UR4StatBaseComponent* victimStat = InVictim->FindComponentByClass<UR4StatBaseComponent>())
			{
				if(FR4StatData* armorStat = victimStat->GetStatByTag<FR4StatData>(TAG_STAT_NORMAL_Armor))
					v_Armor = armorStat->GetBaseValue() + armorStat->GetModifierValue();
			}
		}
	
		// TODO : 주고 받는 측 데미지 증감 계산
		
		// By Instigator //
		// 랜덤 변동성 추가
		retDamageInfo.IncomingDamage *= GetRandomFactor();
		
		// 치명타 계산
		retDamageInfo.bCritical = DetermineCritical(i_CriticalChance, retDamageInfo.IncomingDamage);

		// By Victim //
		// 방어력 계산
		retDamageInfo.IncomingDamage *= CalculateReductionByArmor(v_Armor);
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
