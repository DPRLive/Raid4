// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4UserWidget.h"
#include "R4HpBarWidget.generated.h"

class UR4StackProgressBar;

/**
 * Hp bar widget
 */
UCLASS()
class RAID4_API UR4HpBarWidget : public UR4UserWidget
{
	GENERATED_BODY()

public:
	UR4HpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
public:
	// 최대 체력을 설정
	void SetTotalHp( float InTotalHp );
	
	// 현재 체력을 설정
	void SetCurrentHp( float InCurrentHp );

	// 현재 쉴드량을 설정
	void SetCurrentShieldAmount( float InCurrentShieldAmount );

	// 현재 설정되어 있는 값 Getter
	FORCEINLINE float GetTotalHp() const { return CachedTotalHp; }
	FORCEINLINE float GetCurrentHp() const { return CachedCurrentHp; }
	FORCEINLINE float GetShieldAmount() const { return CachedShieldAmount; }
	
	// Hp Bar Update 시 broadcast
	FSimpleMulticastDelegate OnHpBarUpdate;
	
private:
	// HpBar를 업데이트.
	void _UpdateHpBar() const;
	
private:
	// 체력 바
	UPROPERTY( BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true) )
	TObjectPtr<UR4StackProgressBar> HpBar;

	// 최대 체력 상태를 캐싱
	float CachedTotalHp;
	
	// 현재 체력 상태를 캐싱
	float CachedCurrentHp;
	
	// 현재 방어막 상태를 캐싱
	float CachedShieldAmount;
};