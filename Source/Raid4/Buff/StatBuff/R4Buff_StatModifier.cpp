// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_StatModifier.h"
#include "../../Stat/R4TagStatQueryInterface.h"
#include "../../Stat/R4StatStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_StatModifier)

UR4Buff_StatModifier::UR4Buff_StatModifier()
{
	bDeactivate = false;
	TargetStatTag = FGameplayTag::EmptyTag;
	ModifierType = EOperatorType::Add;
	Value = FR4InteractValueSelector();
	bApplyProportionalAdjustment = false;
	CachedDeltaValue = 0.f;
}

#if WITH_EDITOR
bool UR4Buff_StatModifier::CanEditChange(const FProperty* InProperty) const
{
	bool ret = Super::CanEditChange(InProperty);

	// bApplyProportionalAdjustment를 Current Stat 시에만 변경 가능하도록 제한
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UR4Buff_StatModifier, bApplyProportionalAdjustment))
		ret &= TargetStatTag.MatchesTag(TAG_STAT_CURRENT);

	return ret;
}

void UR4Buff_StatModifier::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// stat tag가 Current가 아니면 bApplyProportionalAdjustment를 강제로 false
	if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UR4Buff_StatModifier, TargetStatTag))
	{
		bApplyProportionalAdjustment &= TargetStatTag.MatchesTag(TAG_STAT_CURRENT);
	}
}
#endif

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_StatModifier::SetupBuff(AActor* InInstigator, AActor* InVictim)
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
bool UR4Buff_StatModifier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;
	
	IR4TagStatQueryInterface* target = Cast<IR4TagStatQueryInterface>(CachedVictim);
	if(target == nullptr)
		return false;

	// 스탯을 찾아서 적용
	if(FR4StatInfo* statData = target->GetStatByTag(TargetStatTag))
	{
		float prevTotalValue = statData->GetTotalValue();

		// 피연산 값 계산
		float value = Value.GetValue(CachedInstigator.Get(), CachedVictim.Get());
		
		// Operator에 따라 연산 처리
		switch (ModifierType)
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

		// Current Stat 비례하도록 변경
		if(bApplyProportionalAdjustment && TargetStatTag.MatchesTag(TAG_STAT_CURRENT) && !FMath::IsNearlyZero(prevTotalValue))
		{
			if(FR4CurrentStatInfo* currStatData = target->GetCurrentStatByTag(TargetStatTag))
				currStatData->SetCurrentValue(currStatData->GetCurrentValue() / prevTotalValue * currStatData->GetTotalValue());
		}

		return true;
	}
	
	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  변경했던 Modifier의 Delta만큼 원래대로 복구
 */
void UR4Buff_StatModifier::Deactivate()
{
	Super::Deactivate();
	
	IR4TagStatQueryInterface* target = Cast<IR4TagStatQueryInterface>(CachedVictim);
	if(target == nullptr)
		return;
	
	float prevTotalValue = 0.f;
	
	// 누적 한 값 돌려주기
	if(FR4StatInfo* statData = target->GetStatByTag(TargetStatTag))
	{
		prevTotalValue = statData->GetTotalValue();
		
		// Operator에 따라 연산 처리
		switch (ModifierType)
		{
		case EOperatorType::Multiply:
			statData->SetMultiplyModifierValue(statData->GetMultiplyModifierValue() / CachedDeltaValue);
			break;	
			
		default: case EOperatorType::Add:
			statData->SetAddModifierValue(statData->GetAddModifierValue() - CachedDeltaValue);
			break;
		}
	}

	// Current Stat 비례하도록 변경
	if(bApplyProportionalAdjustment && TargetStatTag.MatchesTag(TAG_STAT_CURRENT) && !FMath::IsNearlyZero(prevTotalValue))
	{
		if(FR4CurrentStatInfo* statData = target->GetCurrentStatByTag(TargetStatTag))
			statData->SetCurrentValue(statData->GetCurrentValue() / prevTotalValue * statData->GetTotalValue());
	}
	
	CachedDeltaValue = ( ModifierType == EOperatorType::Add ? 0.f : 1.f );
}

/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4Buff_StatModifier::Reset()
{
	Super::Reset();

	CachedDeltaValue = ( ModifierType == EOperatorType::Add ? 0.f : 1.f );
}
