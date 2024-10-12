// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatusBarWidget.h"
#include "../ProgressBar/R4StackProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatusBarWidget)

UR4StatusBarWidget::UR4StatusBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedCurrentHp = 0.f;
	CachedTotalHp = 0.f;
	CachedShieldAmount = 0.f;
}

/**
 * 최대 체력 상태를 업데이트
 */
void UR4StatusBarWidget::SetTotalHp( float InTotalHp )
{
	CachedTotalHp = InTotalHp;
	_UpdateHpBar();
}

/**
 * 현재 체력 상태를 업데이트
 */
void UR4StatusBarWidget::SetCurrentHp( float InCurrentHp )
{
	CachedCurrentHp = InCurrentHp;
	_UpdateHpBar();
}

/**
 * 현재 쉴드량을 업데이트
 */
void UR4StatusBarWidget::SetCurrentShieldAmount( float InCurrentShieldAmount )
{
	CachedShieldAmount = InCurrentShieldAmount;
	_UpdateHpBar();
}

/**
 * status bar를 초기화.
 */
void UR4StatusBarWidget::Clear()
{
	CachedCurrentHp = 0.f;
	CachedTotalHp = 0.f;
	CachedShieldAmount = 0.f;
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
	HpBar->SetTopProgressRatio( CachedCurrentHp / totalAmount );

	// 방어막 부분 갱신
	HpBar->SetBottomProgressRatio( CachedShieldAmount / totalAmount );
}
