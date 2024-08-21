// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatCurrModifier.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatCurrModifier)

UR4Buff_StatCurrModifier::UR4Buff_StatCurrModifier()
{
	TargetStatTag = FGameplayTag::EmptyTag;
	OperatorType = EOperatorType::Add;
	bAllowNegative = false;
	bAllowOverTotalStat = false;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Current Stat와의 피연산자로 사용.
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
	if(FR4CurrentStatInfo* statData = CachedStatComp->GetStatByTag<FR4CurrentStatInfo>(TargetStatTag))
	{
		// Operator에 따라 연산 처리
		float newValue = 0.f;
		switch (OperatorType)
		{
		case EOperatorType::Multiply:
			newValue = statData->GetCurrentValue() * BuffDesc.Value;
			break;	
			
		default: case EOperatorType::Add:
			newValue = statData->GetCurrentValue() + BuffDesc.Value;
			break;
		}

		// 음수 비 허용 시 min을 0으로 최소 값을 제한
		if(!bAllowNegative)
			newValue = FMath::Max(newValue, 0.f);

		// Total Stat Over 제한 시 Total Stat으로 최대 값을 제한  
		if(!bAllowOverTotalStat)
			newValue = FMath::Min(newValue, statData->GetTotalValue());
		
		statData->SetCurrentValue(newValue);
	}
}


/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4Buff_StatCurrModifier::Reset()
{
	Super::Reset();

	CachedStatComp.Reset();
}
