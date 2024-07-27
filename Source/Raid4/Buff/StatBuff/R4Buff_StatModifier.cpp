// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatModifier.h"
#include "../../Stat/R4StatInterface.h"
#include "../../Stat/R4StatBaseComponent.h"
#include "../R4BuffModifyDesc.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatModifier)

UR4Buff_StatModifier::UR4Buff_StatModifier()
{
	StatTag = FGameplayTag::EmptyTag;
	Value = 0.f;
	ValueType = EValueType::Absolute;
	CachedDeltaValue = 0.f;
}

void UR4Buff_StatModifier::ApplyBuff(AActor* InVictim, const FR4BuffModifyDesc& InModifyDesc)
{
	Super::ApplyBuff(InVictim, InModifyDesc);
	
	if(!IsValid(InVictim))
		return;

	if(IR4StatInterface* owner = Cast<IR4StatInterface>(InVictim))
		CachedStatComp = owner->GetStatComponent();

	if(!CachedStatComp.IsValid())
		return;

	// 스탯을 찾아서 적용
	if(FR4StatData* statData = CachedStatComp->GetStatByTag<FR4StatData>(StatTag))
	{
		// 계산
		CachedDeltaValue = Value * InModifyDesc.ValueFactor;

		if(ValueType == EValueType::Percent)
			CachedDeltaValue = statData->GetBaseValue() * CachedDeltaValue / 100.f;

		statData->SetModifierValue(statData->GetModifierValue() + CachedDeltaValue);
	}
}

void UR4Buff_StatModifier::RemoveBuff(AActor* InVictim)
{
	if(!IsValid(InVictim) || !CachedStatComp.IsValid())
		return;
	
	// 스탯을 찾아서 적용
	if(FR4StatData* statData = CachedStatComp->GetStatByTag<FR4StatData>(StatTag))
	{
		statData->SetModifierValue(statData->GetModifierValue() - CachedDeltaValue);
	}
}