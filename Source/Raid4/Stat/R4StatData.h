#pragma once

#include "R4StatData.generated.h"

/**
 * Stat 정의시 사용할 구조체.
 * 기본값 (기본이 되는 값)과 수정자(버프나 아이템 등으로 인한 변동 된 값 Delta)로 구성
 */

// Stat Change Delegate Type
DECLARE_MULTICAST_DELEGATE_OneParam( FOnChangeStatData, float )

USTRUCT( BlueprintType )
struct RAID4_API FR4StatData
{
public:
	GENERATED_BODY()

	FR4StatData() : BaseValue(0.f), ModifierValue(0.f) {}

	// Initializer
	FORCEINLINE void InitStatData(float InBaseValue = 0.f, float InModifierValue = 0.f) { SetBaseValue(InBaseValue); SetModifierValue(InModifierValue); }

	// Getter
	FORCEINLINE float GetBaseValue() const { return BaseValue; }
	FORCEINLINE float GetModifierValue() const { return BaseValue; }

	// Setter
	FORCEINLINE void SetBaseValue(float InBaseValue) { BaseValue = InBaseValue; if( OnChangeBaseValue.IsBound() ) OnChangeBaseValue.Broadcast(BaseValue); }
	FORCEINLINE void SetModifierValue(float InModifierValue) { ModifierValue = InModifierValue; if( OnChangeModifierValue.IsBound() ) OnChangeModifierValue.Broadcast(ModifierValue); }

	// Base Value 변경 delegate
	FOnChangeStatData OnChangeBaseValue;

	// Modifier Value 변경 delegate
	FOnChangeStatData OnChangeModifierValue;
	
private:
	// 기본 값
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float BaseValue;

	// 변동되는 수치 ( 버프나 아이템 등의 이유로 ) 
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float ModifierValue;
};

/**
 * 소모형 Stat 정의시 사용할 구조체. FR4StatData에 CurrentValue가 추가.
 * 소모형이란 Hp 처럼 현재 상태가 추가되어야 하는 스탯의 경우를 말함
 * ex) (기본 Hp, 수정자 Hp, 현재 Hp)
 */
USTRUCT( BlueprintType )
struct RAID4_API FR4ConsumableStatData : public FR4StatData
{
public:
	GENERATED_BODY()

	FR4ConsumableStatData() : CurrentValue(0.f) {}
	
	// Current Value 변경 delegate
	FOnChangeStatData OnChangeCurrentValue;
	
	// Getter
	FORCEINLINE float GetCurrentValue() const { return CurrentValue; }

	// Setter
	FORCEINLINE void SetCurrentValue(float InCurrentValue) { CurrentValue = InCurrentValue; if( OnChangeCurrentValue.IsBound() ) OnChangeCurrentValue.Broadcast(CurrentValue); }
	
protected:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float CurrentValue;
};