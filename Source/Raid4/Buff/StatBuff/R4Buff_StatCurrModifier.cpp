// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
#include "../../Stat/R4TagStatQueryInterface.h"
#include "../../Stat/R4StatStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatCurrModifier)

UR4Buff_StatCurrModifier::UR4Buff_StatCurrModifier()
{
	bDeactivate = false;
	TargetStatTag = FGameplayTag::EmptyTag;
	OperatorType = EOperatorType::Add;
	Value = FR4InteractValueSelector();
	bAllowNegative = false;
	bAllowOverTotalStat = false;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_StatCurrModifier::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	if(!InVictim->GetClass())
		return false;
	
	// R4TagStatQueryInterface 구현이 되었는지 확인
	return bReady && InVictim->GetClass()->ImplementsInterface(UR4TagStatQueryInterface::StaticClass());
}

/**
 *  버프를 적용 ( 스탯을 변경 )
 */
bool UR4Buff_StatCurrModifier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;

	IR4TagStatQueryInterface* target = Cast<IR4TagStatQueryInterface>(CachedVictim);
	if(target == nullptr)
		return false;
	
	// 스탯을 찾아서 적용
	if(FR4CurrentStatInfo* statData = target->GetCurrentStatByTag(TargetStatTag))
	{
		// Operator에 따라 연산 처리
		float newValue = 0.f;
		
		// 피연산 값 계산
		float value = Value.GetValue(CachedInstigator.Get(), CachedVictim.Get());

		switch (OperatorType)
		{
		case EOperatorType::Multiply:
			newValue = statData->GetCurrentValue() * value;
			break;	
			
		default: case EOperatorType::Add:
			newValue = statData->GetCurrentValue() + value;
			break;
		}

		// 음수 비 허용 시 min을 0으로 최소 값을 제한
		if(!bAllowNegative)
			newValue = FMath::Max(newValue, 0.f);

		// Total Stat Over 제한 시 Total Stat으로 최대 값을 제한  
		if(!bAllowOverTotalStat)
			newValue = FMath::Min(newValue, statData->GetTotalValue());
		
		statData->SetCurrentValue(newValue);
		
		return true;
	}

	return false;
}