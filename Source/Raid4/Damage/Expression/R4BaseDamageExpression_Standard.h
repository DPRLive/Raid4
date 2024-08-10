// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BaseDamageExpressionInterface.h"
#include "R4BaseDamageExpression_Standard.generated.h"

/**
 * 표준적인 데미지를 계산하는 클래스.
 * FR4DamageApplyDesc의 Value를 그대로 리턴.
 */
UCLASS()
class RAID4_API UR4BaseDamageExpression_Standard : public UObject, public IR4BaseDamageExpressionInterface
{
	GENERATED_BODY()

public:
	/**
	*  표준적인 데미지를 계산, InValue를 그대로 리턴
	*  @param InInstigator : 데미지를 가하는(Apply) 객체
	*  @param InVictim : 데미지를 입는(Receive) 객체
	*  @param InValue : 데미지에 관한 정보.
	*  @return : InValue를 그대로 리턴
	*/
	virtual float CalculateBaseDamage(const AActor* InInstigator, const AActor* InVictim, float InValue) const override;
};
