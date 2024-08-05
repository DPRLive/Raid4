// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"

#include <GameplayTagContainer.h>

#include "R4Buff_StatModifier.generated.h"

/**
 * Stat Modifier를 변경할 수 있는 버프.
 * Base Stat을 기준으로 하여 BuffDesc의 Value에 의해 계산되며, Modifier Stat에 적용.
 * Stat Comp에 의존.
 */
UCLASS(Blueprintable, ClassGroup=(Buff))
class RAID4_API UR4Buff_StatModifier : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_StatModifier();

protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의.
	virtual void PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc) override;
	
	// 버프가 실제로 할 로직을 정의
	virtual void Activate() override;

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

	// 버프를 Clear하는 로직을 정의
	virtual void Clear() override;
	
private:
	// 무슨 스탯을 변경할 것인지 태그로 설정
	// Base Stat을 기준으로 하여 BuffDesc의 Value에 의해 계산되며, Modifier Stat에 적용.
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat", AllowPrivateAccess = true))
	FGameplayTag StatTag;

	// BuffDesc의 Value가 % 인지 그냥 값인지?
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EValueType ValueType;

	// Modifier에 적용한 delta 값
	float CachedDeltaValue;
};
