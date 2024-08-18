// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>

#include "R4BaseDamageExpressionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4BaseDamageExpressionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base Damage Expression class를 정의 시 사용
 */
class RAID4_API IR4BaseDamageExpressionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  InValue를 기반으로 기본 데미지를 계산 (InValue는 클래스 마다 다르게 사용될 수 있음)
	*  (방어력, 데미지 증감 등이 적용이 안된 기본 수식만 계산된 상태)
	*  CDO를 사용함!
	*  @param InInstigator : 데미지를 가하는(Apply) 객체
	*  @param InVictim : 데미지를 입는(Receive) 객체
	*  @param InValue : 데미지에 관한 정보.
	*/
	virtual float CalculateBaseDamage(const AActor* InInstigator, const AActor* InVictim, float InValue) const = 0;
};
