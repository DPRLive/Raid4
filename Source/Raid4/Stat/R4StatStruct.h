#pragma once

#include "R4StatStruct.generated.h"

// Stat Change Delegate Type
DECLARE_MULTICAST_DELEGATE_TwoParams( FOnChangeStatDataDelegate, float /* InPrevValue */, float /* InNowValue */ )

/**
 * Stat 정의시 사용할 구조체.
 * 기본값 (기본이 되는 값), 덧셈 수정자(덧셈 변동 된 값 Delta), 곱셈 수정자(곱셈 변동 된 값 Delta) 구성
 * TotalValue = ( BaseValue + AddModifierValue ) * MultiplyModifierValue;
 */
USTRUCT( BlueprintType )
struct RAID4_API FR4StatInfo
{
public:
	GENERATED_BODY()

	FR4StatInfo() : BaseValue(0.f), AddModifierValue(0.f), MultiplyModifierValue(1.f) { }

	virtual ~FR4StatInfo() = default;
	
	// Initializer, 0으로 초기화, delegate clear
	FORCEINLINE virtual void InitStatData()
	{
		BaseValue = 0.f; AddModifierValue = 0.f; MultiplyModifierValue = 1.f;
		OnChangeStatDataDelegate.Clear();
	}

	// Getter
	FORCEINLINE float GetBaseValue() const { return BaseValue; }
	FORCEINLINE float GetAddModifierValue() const { return AddModifierValue; }
	FORCEINLINE float GetMultiplyModifierValue() const { return MultiplyModifierValue; }
	// ( BaseValue + AddModifierValue ) * MultiplyModifierValue;
	FORCEINLINE float GetTotalValue() const { return ( BaseValue + AddModifierValue ) * MultiplyModifierValue; }

	// Setter
	FORCEINLINE virtual void SetBaseValue(float InBaseValue)
	{
		float prevValue = GetTotalValue();
		BaseValue = InBaseValue;
		
		if( OnChangeStatDataDelegate.IsBound() )
			OnChangeStatDataDelegate.Broadcast( prevValue, GetTotalValue() );
	}
	
	FORCEINLINE virtual void SetAddModifierValue(float InAddModifierValue)
	{
		float prevValue = GetTotalValue();
		AddModifierValue = InAddModifierValue;
		
		if( OnChangeStatDataDelegate.IsBound() )
			OnChangeStatDataDelegate.Broadcast( prevValue, GetTotalValue() );
	}

	FORCEINLINE virtual void SetMultiplyModifierValue(float InMultiplyModifierValue)
	{
		float prevValue = GetTotalValue();
		MultiplyModifierValue = InMultiplyModifierValue;
		
		if( OnChangeStatDataDelegate.IsBound() )
			OnChangeStatDataDelegate.Broadcast( prevValue, GetTotalValue() );
	}
	
	// Stat 변경 delegate, Total Stat을 broadcast
	FOnChangeStatDataDelegate OnChangeStatDataDelegate;
	
private:
	// 기본 값
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float BaseValue;

	// 덧셈 변동 수치 ( 버프나 아이템 등의 이유로 ) 
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float AddModifierValue;

	// 곱셈 변동 수치 ( 버프나 아이템 등의 이유로 )
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float MultiplyModifierValue;
};

/**
 * 현재 값을 따로 관리해야 할 시 사용할 구조체.
 * FR4StatData에 CurrentValue가 추가.
 * ex) Hp 처럼 현재 상태가 추가되어야 하는 스탯의 경우 등.
 * ex) (기본 Hp, 수정자 Hp, 현재 Hp)
 */
USTRUCT( BlueprintType )
struct RAID4_API FR4CurrentStatInfo : public FR4StatInfo
{
public:
	GENERATED_BODY()

	FR4CurrentStatInfo() : CurrentValue(0.f) {}

	virtual ~FR4CurrentStatInfo() override = default;
	
	// Initializer, 0으로 초기화, delegate clear
	FORCEINLINE virtual void InitStatData() override
	{
		Super::InitStatData();
		CurrentValue = 0.f; OnChangeCurrentValueDelegate.Clear();
	}
	
	// Getter
	FORCEINLINE float GetCurrentValue() const { return CurrentValue; }
	
	FORCEINLINE void SetCurrentValue(float InCurrentValue)
	{
		float prevValue = CurrentValue;
		
		CurrentValue = InCurrentValue;
		
		if( OnChangeCurrentValueDelegate.IsBound() )
			OnChangeCurrentValueDelegate.Broadcast( prevValue, CurrentValue );
	}

	// Current Value 변경 delegate
	FOnChangeStatDataDelegate OnChangeCurrentValueDelegate;
	
private:
	// 현재 값
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	float CurrentValue;
};