#pragma once

#include "R4StatStruct.generated.h"


// Stat Change Delegate Type
DECLARE_MULTICAST_DELEGATE_TwoParams( FOnChangeStatDataDelegate, float /* BaseStat */, float /* ModifierStat */ )
DECLARE_MULTICAST_DELEGATE_OneParam( FOnChangeCurrentStatDataDelegate, float /* Current Stat */);

/**
 * Stat 정의시 사용할 구조체.
 * 기본값 (기본이 되는 값)과 수정자(버프나 아이템 등으로 인한 변동 된 값 Delta)로 구성
 */
USTRUCT( BlueprintType )
struct RAID4_API FR4StatInfo
{
public:
	GENERATED_BODY()

	FR4StatInfo() : BaseValue(0.f), ModifierValue(0.f) { }

	virtual ~FR4StatInfo() = default;
	
	// Initializer, 0으로 초기화, delegate clear
	FORCEINLINE virtual void InitStatData()
	{ BaseValue = 0.f; ModifierValue = 0.f; OnChangeStatDataDelegate.Clear();}

	// Getter
	FORCEINLINE float GetBaseValue() const { return BaseValue; }
	FORCEINLINE float GetModifierValue() const { return ModifierValue; }

	// Setter
	FORCEINLINE virtual void SetBaseValue(float InBaseValue)
	{
		BaseValue = InBaseValue;
		if( OnChangeStatDataDelegate.IsBound() )
			OnChangeStatDataDelegate.Broadcast(BaseValue, ModifierValue);
	}
	
	FORCEINLINE virtual void SetModifierValue(float InModifierValue)
	{
		ModifierValue = InModifierValue;
		if( OnChangeStatDataDelegate.IsBound() )
			OnChangeStatDataDelegate.Broadcast(BaseValue, ModifierValue);
	}

	// Stat 변경 delegate
	FOnChangeStatDataDelegate OnChangeStatDataDelegate;
	
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
struct RAID4_API FR4ConsumableStatInfo : public FR4StatInfo
{
public:
	GENERATED_BODY()

	FR4ConsumableStatInfo() : CurrentValue(0.f) {}

	virtual ~FR4ConsumableStatInfo() override = default;
	
	// Initializer, 0으로 초기화, delegate clear
	FORCEINLINE virtual void InitStatData() override
	{
		Super::InitStatData();
		CurrentValue = 0.f; OnChangeCurrentValueDelegate.Clear();
	}
	
	// Getter
	FORCEINLINE float GetCurrentValue() const { return CurrentValue; }

	// Setter
	// Current Value가 Base + ModifyValue를 넘지 못하도록 Clamp
	FORCEINLINE virtual void SetBaseValue(float InBaseValue) override
	{
		Super::SetBaseValue(InBaseValue);
		SetCurrentValue(FMath::Min(CurrentValue, GetBaseValue() + GetModifierValue()));
	}

	// Current Value가 Base + ModifyValue를 넘지 못하도록 Clamp
	FORCEINLINE virtual void SetModifierValue(float InModifierValue) override
	{
		Super::SetModifierValue(InModifierValue);
		SetCurrentValue(FMath::Min(CurrentValue, GetBaseValue() + GetModifierValue()));
	}
	
	FORCEINLINE void SetCurrentValue(float InCurrentValue)
	{
		CurrentValue = InCurrentValue;
		if( OnChangeCurrentValueDelegate.IsBound() )
			OnChangeCurrentValueDelegate.Broadcast(CurrentValue);
	}

	// Current Value 변경 delegate
	FOnChangeCurrentStatDataDelegate OnChangeCurrentValueDelegate;
	
private:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float CurrentValue;
};