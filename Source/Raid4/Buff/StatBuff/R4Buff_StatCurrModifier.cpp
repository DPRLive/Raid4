// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
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
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Current Stat에 증감할 값, OperandType, ValueType 등에 따라 다르게 계산 될 수 있음
 */
void UR4Buff_StatCurrModifier::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	Super::PreActivate(InInstigator, InVictim, InBuffDesc);

	// 버프 받을 객체의 StatComp를 캐싱
	if(CachedVictim.IsValid())
		CachedStatComp = CachedVictim->GetComponentByClass<UR4StatBaseComponent>();
	
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

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 */
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

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_StatCurrModifier::Clear()
{
	Super::Clear();

	CachedStatComp.Reset();
	CachedDeltaValue = 0.f;
}
