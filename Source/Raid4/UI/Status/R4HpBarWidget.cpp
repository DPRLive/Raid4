// Fill out your copyright notice in the Description page of Project Settings.


#include "R4HpBarWidget.h"
#include "R4HpBarWidgetInterface.h"
#include "../ProgressBar/R4StackProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4HpBarWidget)

UR4HpBarWidget::UR4HpBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedCurrentHp = 0.f;
	CachedTotalHp = 0.f;
	CachedShieldAmount = 0.f;
}

void UR4HpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( !OwningActor.IsValid() )
		return;

	if ( IR4HpBarWidgetInterface* owner = Cast<IR4HpBarWidgetInterface>( OwningActor ) )
		owner->SetupHpBarWidget( this );
}

/**
 * 최대 체력 상태를 업데이트
 */
void UR4HpBarWidget::SetTotalHp( float InTotalHp )
{
	CachedTotalHp = InTotalHp;
	_UpdateHpBar();
}

/**
 * 현재 체력 상태를 업데이트
 */
void UR4HpBarWidget::SetCurrentHp( float InCurrentHp )
{
	CachedCurrentHp = InCurrentHp;
	_UpdateHpBar();
}

/**
 * 현재 쉴드량을 업데이트
 */
void UR4HpBarWidget::SetCurrentShieldAmount( float InCurrentShieldAmount )
{
	CachedShieldAmount = InCurrentShieldAmount;
	_UpdateHpBar();
}

/**
 * HpBar를 업데이트.
 */
void UR4HpBarWidget::_UpdateHpBar() const
{
	if ( !IsValid( HpBar ) )
		return;
	
	// division by zero 방지
	float totalAmount = CachedTotalHp + CachedShieldAmount + KINDA_SMALL_NUMBER;

	// 체력 부분 갱신
	HpBar->SetTopProgressRatio( CachedCurrentHp / totalAmount );

	// 방어막 부분 갱신
	HpBar->SetBottomProgressRatio( CachedShieldAmount / totalAmount );

	// broadcast
	if( OnHpBarUpdate.IsBound() )
		OnHpBarUpdate.Broadcast();
}
