// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
#include "../../Stat/R4StatInterface.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatCurrModifier)

UR4Buff_StatCurrModifier::UR4Buff_StatCurrModifier()
{
	StatTag = FGameplayTag::EmptyTag;
	OperandType = EStatOperandType::Current;
	ValueType = EValueType::Absolute;
	bAllowNegative = false;
	CachedDeltaValue = 0.f;
}

/**
 *  버프 적용 전 세팅
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 */
void UR4Buff_StatCurrModifier::PreActivate(AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	Super::PreActivate(InVictim, InBuffDesc);
	
	if(!IsValid(InVictim))
		return;

	if(IR4StatInterface* owner = Cast<IR4StatInterface>(InVictim))
		CachedStatComp = owner->GetStatComponent();

	CachedDeltaValue = 0.f;
}

/**
 *  버프를 적용 ( 스탯을 변경 )
 */
void UR4Buff_StatCurrModifier::Activate()
{
	Super::Activate();

	if(!CachedStatComp.IsValid())
		return;
	
	// 스탯을 찾아서 적용
	if(FR4ConsumableStatData* statData = CachedStatComp->GetStatByTag<FR4ConsumableStatData>(StatTag))
	{
		// 계산
		float value = BuffDesc.Value;
		
		if(ValueType == EValueType::Percent)
		{
			float operand = 0.f;
			
			switch (OperandType)
			{
			case EStatOperandType::Base:
				operand = statData->GetBaseValue();
				break;
			
			case EStatOperandType::Total:
				operand = statData->GetBaseValue() + statData->GetModifierValue();
				break;

			default: case EStatOperandType::Current:
				operand = statData->GetCurrentValue();
				break;
			}

			// % 계산
			value = operand * value / 100.f;
		}

		float newValue = statData->GetCurrentValue() + value;

		// 음수 비 허용 시
		if(bAllowNegative == false && newValue < 0.f)
		{
			newValue = 0.f;
			value = statData->GetCurrentValue();
		}
		
		statData->SetCurrentValue(newValue);
		
		// 적용한 값을 누적
		CachedDeltaValue += value;
	}
}

void UR4Buff_StatCurrModifier::Deactivate()
{
	Super::Deactivate();

	if(!CachedStatComp.IsValid())
		return;
	
	// 누적 한 값 돌려주기
	if(FR4ConsumableStatData* statData = CachedStatComp->GetStatByTag<FR4ConsumableStatData>(StatTag))
	{
		float value = statData->GetCurrentValue() + CachedDeltaValue;

		// 최대를 넘어가지 않도록 조정
		statData->SetCurrentValue(FMath::Max(value, statData->GetBaseValue() + statData->GetModifierValue()));
	}

	CachedDeltaValue = 0.f;
}

void UR4Buff_StatCurrModifier::Clear()
{
	Super::Clear();

	CachedStatComp.Reset();
	CachedDeltaValue = 0.f;
}
