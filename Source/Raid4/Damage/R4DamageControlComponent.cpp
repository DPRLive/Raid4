// Fill out your copyright notice in the Description page of Project Settings.


#include "R4DamageControlComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4DamageControlComponent)

UR4DamageControlComponent::UR4DamageControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CachedDamage = 0.f;
	CachedCalculatedDamage = 0.f;
}


void UR4DamageControlComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  입히고 싶은 데미지를 적용 (0 이하로 내려가지 않음)
 *  @param InDamage : 입히고 싶은 데미지
 */
void UR4DamageControlComponent::PushNewDamage(float InDamage)
{
	CachedDamage = InDamage;

	if(CachedDamage < 0.f)
		CachedDamage = 0.f;
}

/**
 *  데미지를 수정한다. (0 이하로 내려가지 않음)
 *  @param InDeltaDamage : 수정하고 싶은 데미지 Delta. (- 데미지 감소 / + 데미지 증가)
 */
void UR4DamageControlComponent::ModifyDamage(float InDeltaDamage)
{
	CachedCalculatedDamage += InDeltaDamage;

	if(CachedCalculatedDamage < 0.f)
		CachedCalculatedDamage = 0.f;
}

/**
 *  계산된 데미지를 return (0 이하로 내려가지 않음)
 */
float UR4DamageControlComponent::GetCalculatedDamage()
{
	CachedCalculatedDamage = CachedDamage;

	if(OnNewDamageDelegate.IsBound())
		OnNewDamageDelegate.Broadcast(CachedDamage);

	return CachedCalculatedDamage;
}
