// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatusBarWidget.h"
#include "R4StatusBarInterface.h"
#include "../ProgressBar/R4StackProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatusBarWidget)

UR4StatusBarWidget::UR4StatusBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedCurrentHp = 0.f;
	CachedTotalHp = 0.f;
	CachedShieldAmount = 0.f;
}

void UR4StatusBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!OwningActor.IsValid())
		return;

	if(IR4StatusBarInterface* owner = Cast<IR4StatusBarInterface>(OwningActor))
		owner->SetupStatusBarWidget(this);
}

/**
 * 현재 체력 상태를 업데이트
 */
void UR4StatusBarWidget::UpdateCurrentHp(float InCurrentHp)
{
	CachedCurrentHp = InCurrentHp;
	_UpdateHpBar();
}

/**
 * 현재 쉴드량을 업데이트
 */
void UR4StatusBarWidget::UpdateCurrentShieldAmount(float InCurrentShieldAmount)
{
	CachedShieldAmount = InCurrentShieldAmount;
	_UpdateHpBar();
}

/**
 * 최대 체력 상태를 업데이트
 */
void UR4StatusBarWidget::UpdateTotalHp(float InTotalHp)
{
	CachedTotalHp = InTotalHp;
	_UpdateHpBar();
}

/**
 * HpBar를 업데이트.
 */
void UR4StatusBarWidget::_UpdateHpBar() const
{
	// division by zero 방지
	float totalAmount = CachedTotalHp + CachedShieldAmount + KINDA_SMALL_NUMBER;
	
	// 체력 부분 갱신
	HpBar->SetTopProgressRatio(CachedCurrentHp / totalAmount);
		
	// 방어막 부분 갱신
	HpBar->SetBottomProgressRatio(CachedShieldAmount / totalAmount);
}
