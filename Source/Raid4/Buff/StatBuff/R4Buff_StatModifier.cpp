// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatModifier.h"
#include "../../Stat/R4StatInterface.h"
#include "../../Stat/R4StatBaseComponent.h"
#include "../R4BuffDesc.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatModifier)

UR4Buff_StatModifier::UR4Buff_StatModifier()
{
	StatTag = FGameplayTag::EmptyTag;
	ValueType = EValueType::Absolute;
	CachedDeltaValue = 0.f;
}

/**
 *  버프 적용 전 세팅
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Base Stat을 기준으로 하여 BuffDesc의 Value에 의해 증감할 값을 계산.
 */
void UR4Buff_StatModifier::PreActivate(AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	Super::PreActivate(InVictim, InBuffDesc);
	
	if(!IsValid(InVictim))
		return;

	if(IR4StatInterface* owner = Cast<IR4StatInterface>(InVictim))
		CachedStatComp = owner->GetStatComponent();
}

/**
 *  버프를 적용 ( 스탯을 변경 )
 */
void UR4Buff_StatModifier::Activate()
{
	Super::Activate();

	if(!CachedStatComp.IsValid())
		return;
	
	// 스탯을 찾아서 적용
	if(FR4StatData* statData = CachedStatComp->GetStatByTag<FR4StatData>(StatTag))
	{
		// 계산
		float value = BuffDesc.Value;

		if(ValueType == EValueType::Percent)
			value = statData->GetBaseValue() * value / 100.f;

		statData->SetModifierValue(statData->GetModifierValue() + value);

		// 적용한 값을 누적
		CachedDeltaValue += value;
	}
}

void UR4Buff_StatModifier::Deactivate()
{
	Super::Deactivate();

	if(!CachedStatComp.IsValid())
		return;
	
	// 누적 한 값 돌려주기
	if(FR4StatData* statData = CachedStatComp->GetStatByTag<FR4StatData>(StatTag))
	{
		statData->SetModifierValue(statData->GetModifierValue() - CachedDeltaValue);
	}
}