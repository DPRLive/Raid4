// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatCurrModifier)

UR4Buff_StatCurrModifier::UR4Buff_StatCurrModifier()
{
	bDeactivate = false;
	TargetStatTag = FGameplayTag::EmptyTag;
	OperatorType = EOperatorType::Add;
	Value = FR4ValueSelector();
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

	// 버프 받을 객체의 StatComp를 캐싱
	if(CachedVictim.IsValid())
		CachedStatComp = CachedVictim->FindComponentByClass<UR4StatBaseComponent>();
	
	return bReady && CachedStatComp.IsValid();
}

/**
 *  버프를 적용 ( 스탯을 변경 )
 */
bool UR4Buff_StatCurrModifier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;
	
	if(!CachedStatComp.IsValid())
		return false;
	
	// 스탯을 찾아서 적용
	if(FR4CurrentStatInfo* statData = CachedStatComp->GetStatByTag<FR4CurrentStatInfo>(TargetStatTag))
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


/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4Buff_StatCurrModifier::Reset()
{
	Super::Reset();

	CachedStatComp.Reset();
}
