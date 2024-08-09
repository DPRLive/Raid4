// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DamageCalculatorInterface.h"

#include <UObject/NoExportTypes.h>

#include "R4DamageCalculator_Standard.generated.h"

/**
 * 표준적인 데미지를 계산하는 클래스.
 * 기본 공격력, 방어력 (적), 치명타 확률, 치명타 배율, 랜덤 변동성, 데미지 계수
 * 를 사용하여 계산.
 */
UCLASS()
class RAID4_API UR4DamageCalculator_Standard : public UObject, public IR4DamageCalculatorInterface
{
	GENERATED_BODY()

public:
	// 기본 공격력, 방어력 (적), 치명타 확률, 치명타 배율, 랜덤 변동성, 데미지 계수를 사용하여 계산.
	virtual FR4DamageReceiveInfo CalculateDamage(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc) const override;
};
