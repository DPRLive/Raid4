// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BaseDamageExpression_StatProportion.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BaseDamageExpression_StatProportion)

UR4BaseDamageExpression_StatProportion::UR4BaseDamageExpression_StatProportion()
{
	TargetStatOwner = ETargetStatOwner::Victim;
	StatTag = FGameplayTag::EmptyTag;
	OperandType = EStatOperandType::Total;
}

/**
*  특정 '스탯'에 비례한 기본 데미지를 계산하는 클래스.
*  ETargetStatOwner 의 Stat중 TAG에 맞는 Stat을 기반으로 계산 진행
*  FR4DamageApplyDesc의 Value를 특정 Stat에 대한 비례 데미지 계산 시 '비율'로 사용.
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InValue : 데미지에 관한 정보.
*  @return : 특정 '스탯'에 비례한 데미지
*/
float UR4BaseDamageExpression_StatProportion::CalculateBaseDamage(const AActor* InInstigator, const AActor* InVictim, float InValue) const
{
	const AActor* target = (TargetStatOwner == ETargetStatOwner::Instigator ? InInstigator : InVictim);
	if(!IsValid(target))
		return 0.f;
	
	if (UR4StatBaseComponent* targetStatComp = target->FindComponentByClass<UR4StatBaseComponent>())
	{
		switch (OperandType)
		{
		case EStatOperandType::Base:
			if (FR4StatData* stat = targetStatComp->GetStatByTag<FR4StatData>(StatTag))
				return stat->GetBaseValue() * InValue;
			break;

		case EStatOperandType::Current:
			if (FR4ConsumableStatData* stat = targetStatComp->GetStatByTag<FR4ConsumableStatData>(StatTag))
				return stat->GetCurrentValue() * InValue;
			break;

		default: case EStatOperandType::Total:
			if (FR4StatData* stat = targetStatComp->GetStatByTag<FR4StatData>(StatTag))
				return (stat->GetBaseValue() + stat->GetModifierValue()) * InValue;
			break;
		}
	}
	return 0.f;
}
