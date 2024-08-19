// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4UserWidget.h"
#include "R4StatusBarWidget.generated.h"

class UR4StackProgressBar;

/**
 * 이름과 HP를 표기해주는 상태바.
 */
UCLASS()
class RAID4_API UR4StatusBarWidget : public UR4UserWidget
{
	GENERATED_BODY()

public:
	UR4StatusBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	// 현재 체력을 업데이트
	void UpdateCurrentHp(float InCurrentHp);

	// 현재 쉴드량을 업데이트
	void UpdateCurrentShieldAmount(float InCurrentShieldAmount);
	
	// 최대 체력을 업데이트
	void UpdateTotalHp(float InTotalHp);

private:
	// HpBar를 업데이트.
	void _UpdateHpBar() const;
	
private:
	// 체력 바
	UPROPERTY( BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true) )
	TObjectPtr<UR4StackProgressBar> HpBar;
	
	// 현재 체력 상태를 캐싱
	float CachedCurrentHp;

	// 최대 체력 상태를 캐싱
	float CachedTotalHp;
	
	// 현재 방어막 상태를 캐싱
	float CachedShieldAmount;
};
