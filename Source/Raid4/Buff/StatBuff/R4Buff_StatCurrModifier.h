// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_StatCurrModifier.generated.h"

class UR4StatBaseComponent;

/**
 * % 기반 계산 시 어떤것을 피연산자로 할 것 인지 ?
 */
UENUM( BlueprintType )
enum class EStatOperandType : uint8
{
	Base			UMETA( DisplayName = "기본 스탯" ),
	Total			UMETA( DisplayName = "스탯 총합" ),
	Current			UMETA( DisplayName = "현재 스탯" ),
};

/**
 * 'Current' Stat을 변경할 수 있는 버프. ( Consumable Stat에 사용 )
 * Stat Comp에 의존.
 */
UCLASS(Blueprintable, ClassGroup=(Buff))
class RAID4_API UR4Buff_StatCurrModifier : public UR4BuffBase
{
	GENERATED_BODY()
	
public:
	UR4Buff_StatCurrModifier();
	
protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의.
	virtual void PreActivate(AActor* InVictim, const FR4BuffDesc* InBuffDesc = nullptr) override;

	// 버프가 실제로 할 로직을 정의
	virtual void Activate() override;

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

private:
	// 무슨 스탯을 변경할 것인지 태그로 설정
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat.Consumable", AllowPrivateAccess = true))
	FGameplayTag StatTag;

	// % 선택 시 어떤 것을 기반으로 %를 계산 할 것인지 설정
	UPROPERTY( EditDefaultsOnly, meta= ( EditCondition = "ValueType == EValueType::Percent", EditConditionHides, AllowPrivateAccess = true))
	EStatOperandType OperandType;

	// BuffDesc의 Value가 % 인지 그냥 값인지?
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EValueType ValueType;

	// Current Value가 음수로 내려가는 것을 허용할 것인지
	UPROPERTY( EditDefaultsOnly, Category = "BuffInfo",  meta = (AllowPrivateAccess = true))
	uint8 bAllowNegative:1;
	
	// 영향을 줄 StatComp
	TWeakObjectPtr<UR4StatBaseComponent> CachedStatComp;

	// 적용한 delta 값
	float CachedDeltaValue;
};
