// Fill out your copyright notice in the Description page of Project Settings.


#include "R4InteractValueCalculator_StatBase.h"
#include "../../Stat/R4TagStatQueryInterface.h"
#include "../../Stat/R4StatStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4InteractValueCalculator_StatBase)

UR4InteractValueCalculator_StatBase::UR4InteractValueCalculator_StatBase()
{
	TargetStatOwner = ETargetType::Instigator;
	StatTag = FGameplayTag::EmptyTag;
	OperandType = EStatOperandType::Total;
	Proportion = 1.f;
}

/**
*  Stat에 기반한 Value를 계산.
*  @param InInstigator : 가해자 객체
*  @param InVictim : 피해자 객체
*  @return : Stat에 기반하여 계산 된 Value
*/
float UR4InteractValueCalculator_StatBase::CalculateInteractValue(const AActor* InInstigator, const AActor* InVictim) const
{
	const AActor* target = (TargetStatOwner == ETargetType::Instigator ? InInstigator : InVictim);
	if(!IsValid(target))
		return 0.f;
	
	if (const IR4TagStatQueryInterface* queryTarget = Cast<IR4TagStatQueryInterface>(target))
	{
		switch (OperandType)
		{
		case EStatOperandType::Base:
			if (FR4StatInfo* stat = queryTarget->GetStatByTag(StatTag))
				return stat->GetBaseValue() * Proportion;
			break;

		case EStatOperandType::Current:
			if (FR4CurrentStatInfo* stat = queryTarget->GetCurrentStatByTag(StatTag))
				return stat->GetCurrentValue() * Proportion;
			break;

		default: case EStatOperandType::Total:
			if (FR4StatInfo* stat = queryTarget->GetStatByTag(StatTag))
				return stat->GetTotalValue() * Proportion;
			break;
		}
	}
	
	return 0.f;
}
