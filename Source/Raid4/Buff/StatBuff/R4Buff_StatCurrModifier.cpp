// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatCurrModifier)

UR4Buff_StatCurrModifier::UR4Buff_StatCurrModifier()
{
	StatTag = FGameplayTag::EmptyTag;
	OperandType = EStatOperandType::Current;
	ValueType = EValueType::Constant;
	OperatorType = EOperatorType::Add;
	bAllowNegative = false;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Current Stat에 증감할 값, OperandType, ValueType 등에 따라 다르게 계산 될 수 있음
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_StatCurrModifier::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	bool bReady = Super::PreActivate(InInstigator, InVictim, InBuffDesc);

	// 버프 받을 객체의 StatComp를 캐싱
	if(CachedVictim.IsValid())
		CachedStatComp = CachedVictim->FindComponentByClass<UR4StatBaseComponent>();

	return bReady && CachedStatComp.IsValid();
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
	if(FR4CurrentStatInfo* statData = CachedStatComp->GetStatByTag<FR4CurrentStatInfo>(StatTag))
	{
		// 계산
		float value = BuffDesc.Value;
		
		if(ValueType == EValueType::Percent)
		{
			float operand = 0.f;

			// % 일 시 피연산자 정하기
			switch (OperandType)
			{
			case EStatOperandType::Base:
				operand = statData->GetBaseValue();
				break;
			
			case EStatOperandType::Total:
				operand = statData->GetTotalValue();
				break;

			default: case EStatOperandType::Current:
				operand = statData->GetCurrentValue();
				break;
			}

			// % 계산
			value = operand * value / 100.f;
		}

		// Operator에 따라 연산 처리
		float newValue = 0.f;
		switch (OperatorType)
		{
		case EOperatorType::Multiply:
			newValue = statData->GetCurrentValue() * value;
			break;	
			
		default: case EOperatorType::Add:
			newValue = statData->GetCurrentValue() + value;
			break;
		}

		// 음수 비 허용 시, 0에서 멈춤
		if(bAllowNegative == false && newValue < 0.f)
			newValue = 0.f;
		
		statData->SetCurrentValue(newValue);
	}
}


/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_StatCurrModifier::Clear()
{
	Super::Clear();

	CachedStatComp.Reset();
}
