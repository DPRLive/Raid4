// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4DamageStruct.h"

#include <UObject/Interface.h>

#include "R4DamageCalculatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4DamageCalculatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Damage Calculator class를 정의 시 사용
 */
class RAID4_API IR4DamageCalculatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출
	*  @param InInstigator : 데미지를 가하는(Apply) 객체
	*  @param InVictim : 데미지를 입는(Receive) 객체
	*  @param InDamageDesc : 데미지에 관한 정보.
	*/
	virtual FR4DamageReceiveInfo CalculateDamage(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc) const = 0;
};
