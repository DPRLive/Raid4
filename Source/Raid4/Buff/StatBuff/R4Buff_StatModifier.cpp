// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatModifier.h"
#include "../../Stat/R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatModifier)

UR4Buff_StatModifier::UR4Buff_StatModifier()
{
	StatTag = FGameplayTag::EmptyTag;
	ValueType = EValueType::Constant;
	OperatorType = EOperatorType::Add;
	CachedDeltaValue = 0.f;
}

// #if WITH_EDITOR
// void UR4Buff_StatModifier::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
// {
// 	Super::PostEditChangeProperty(PropertyChangedEvent);
//
// 	// stat tag에 따라서 bApplyProportionalAdjustment 옵션을 보여주도록 설정.
// 	if(PropertyChangedEvent.GetPropertyName() == TEXT("StatTag"))
// 	{
// 		bShowCurrentStatOption = StatTag.MatchesTag(TAG_STAT_CURRENT);
// 	}
// }
// #endif

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Base Stat을 기준으로 하여 BuffDesc의 Value에 의해 증감할 값을 계산.
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_StatModifier::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	bool bReady = Super::PreActivate(InInstigator, InVictim, InBuffDesc);

	// 버프 받을 객체의 StatComp를 캐싱
	if(CachedVictim.IsValid())
		CachedStatComp = CachedVictim->FindComponentByClass<UR4StatBaseComponent>();
	
	CachedDeltaValue = OperatorType == EOperatorType::Add ? 0.f : 1.f;

	return bReady && CachedStatComp.IsValid();
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
	if(FR4StatInfo* statData = CachedStatComp->GetStatByTag<FR4StatInfo>(StatTag))
	{
		// 계산
		float value = BuffDesc.Value;

		if(ValueType == EValueType::Percent)
			value = statData->GetBaseValue() * value / 100.f;
		
		// Operator에 따라 연산 처리
		switch (OperatorType)
		{
		case EOperatorType::Multiply:
			statData->SetMultiplyModifierValue(statData->GetMultiplyModifierValue() * value);
			CachedDeltaValue *= value;
			break;	
			
		default: case EOperatorType::Add:
			statData->SetAddModifierValue(statData->GetAddModifierValue() + value);
			CachedDeltaValue += value;
			break;
		}
	}

	// Current Stat 변경
	// if(bApplyProportionalAdjustment)
	// {
	// 	if(FR4CurrentStatInfo* statData = CachedStatComp->GetStatByTag<FR4CurrentStatInfo>(StatTag))
	// 		statData->
	// }
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 */
void UR4Buff_StatModifier::Deactivate()
{
	Super::Deactivate();
	
	if(!CachedStatComp.IsValid())
		return;
	
	// 누적 한 값 돌려주기
	if(FR4StatInfo* statData = CachedStatComp->GetStatByTag<FR4StatInfo>(StatTag))
	{
		// Operator에 따라 연산 처리
		switch (OperatorType)
		{
		case EOperatorType::Multiply:
			statData->SetMultiplyModifierValue(statData->GetMultiplyModifierValue() / CachedDeltaValue);
			break;	
			
		default: case EOperatorType::Add:
			statData->SetAddModifierValue(statData->GetAddModifierValue() - CachedDeltaValue);
			break;
		}
	}

	CachedDeltaValue = OperatorType == EOperatorType::Add ? 0.f : 1.f;
}

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_StatModifier::Clear()
{
	Super::Clear();

	CachedStatComp.Reset();
	CachedDeltaValue = OperatorType == EOperatorType::Add ? 0.f : 1.f;
}
