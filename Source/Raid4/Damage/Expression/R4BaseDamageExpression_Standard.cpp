// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BaseDamageExpression_Standard.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BaseDamageExpression_Standard)

/**
*  표준적인 데미지를 계산, InValue를 그대로 리턴
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InValue : 데미지에 관한 정보.
*  @return : InValue를 그대로 리턴
*/
float UR4BaseDamageExpression_Standard::CalculateBaseDamage(const AActor* InInstigator, const AActor* InVictim, float InValue) const
{
	return InValue;
}
