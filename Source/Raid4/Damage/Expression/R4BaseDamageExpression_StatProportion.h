// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BaseDamageExpressionInterface.h"
#include "R4BaseDamageExpression_StatProportion.generated.h"

/**
 *  누구의 스탯이 기준인지
 */
UENUM( BlueprintType )
enum class ETargetStatOwner : uint8
{
	Instigator		UMETA( DisplayName = "데미지 가해자 객체" ),
	Victim			UMETA( DisplayName = "데미지 피해자 객체" ),
};

/**
 * 특정 '스탯'에 비례한 기본 데미지를 계산하는 클래스.
 * ETargetStatOwner 의 Stat중 TAG에 맞는 Stat을 기반으로 계산 진행
 * FR4DamageApplyDesc의 Value를 특정 Stat에 대한 비례 데미지 계산 시 '비율'로 사용.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(DamageExpression) )
class RAID4_API UR4BaseDamageExpression_StatProportion : public UObject, public IR4BaseDamageExpressionInterface
{
	GENERATED_BODY()

public:
	UR4BaseDamageExpression_StatProportion();
	
	/**
	*  특정 '스탯'에 비례한 기본 데미지를 계산하는 클래스.
	*  ETargetStatOwner 의 Stat중 TAG에 맞는 Stat을 기반으로 계산 진행
	*  FR4DamageApplyDesc의 Value를 특정 Stat에 대한 비례 데미지 계산 시 '비율'로 사용.
	*  @param InInstigator : 데미지를 가하는(Apply) 객체
	*  @param InVictim : 데미지를 입는(Receive) 객체
	*  @param InValue : 데미지에 관한 정보.
	*  @return : 특정 '스탯'에 비례한 데미지
	*/
	virtual float CalculateBaseDamage(const AActor* InInstigator, const AActor* InVictim, float InValue) const override;

private:
	// 상대의 스탯인지 나의 스탯인지
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ETargetStatOwner TargetStatOwner;
	
	// 비례 데미지 계산 시 기반이 되는 스탯
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat", AllowPrivateAccess = true))
	FGameplayTag StatTag;

	// 스탯 피연산자 타입
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EStatOperandType OperandType;
};