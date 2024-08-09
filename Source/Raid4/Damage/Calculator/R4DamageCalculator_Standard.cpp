// Fill out your copyright notice in the Description page of Project Settings.


#include "R4DamageCalculator_Standard.h"

#include "../../Stat/R4StatBaseComponent.h"
#include "../../Util/UtilDamage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4DamageCalculator_Standard)

/**
*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출
*  이 클래스는 (기본 공격력, 방어력 (적), 치명타 확률, 치명타 배율, 랜덤 변동성, 데미지 계수)를 사용하여 계산.
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InDamageDesc : 데미지에 관한 정보.
*/
FR4DamageReceiveInfo UR4DamageCalculator_Standard::CalculateDamage(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc) const
{
	FR4DamageReceiveInfo retDamageInfo;
	
	// 계산에 필요한 변수들 Instigator와 Victim의 StatComp에서 찾기 i : instigator, v : victim
	float i_AttackPower = 0.f, i_CriticalChance = 0.f, v_Armor = 0.f;

	if(IsValid(InInstigator))
	{
		if(UR4StatBaseComponent* instigatorStat = InInstigator->FindComponentByClass<UR4StatBaseComponent>())
		{
			if(FR4StatData* attackStat = instigatorStat->GetStatByTag<FR4StatData>(TAG_STAT_NORMAL_AttackPower))
				i_AttackPower = attackStat->GetBaseValue() + attackStat->GetModifierValue();

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

	// 데미지 = 기본 데미지 + 공격력 * 계수 
	retDamageInfo.IncomingDamage = InDamageDesc.Value + (i_AttackPower * InDamageDesc.AttackPowerFactor);

	// 고정 데미지가 아닐시 데미지 계산
	if (!InDamageDesc.bFixedDamage)
	{
		// 방어력 계산
		retDamageInfo.IncomingDamage *= UtilDamage::CalculateReductionByArmor(v_Armor);

		// 랜덤 변동성 추가
		retDamageInfo.IncomingDamage *= UtilDamage::GetRandomFactor();
		
		// 치명타 계산
		retDamageInfo.bCritical = UtilDamage::DetermineCritical(i_CriticalChance, retDamageInfo.IncomingDamage);
	}

	return retDamageInfo;
}
