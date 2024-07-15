// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatComponent.h"
#include "R4StatRow.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatComponent)

UR4StatComponent::UR4StatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

/**
 *	컴포넌트 초기화
 */
void UR4StatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);
}

/**
 *  begin play
 */
void UR4StatComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Replicate 설정
 */
void UR4StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate all players
	DOREPLIFETIME(UR4StatComponent, Hp);
	
	// Replicate Only Owner
	DOREPLIFETIME_CONDITION(UR4StatComponent, HpRegenPerSec, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, AttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, Armor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, CoolDownReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, CriticalChance, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, BaseAttackSpeed, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, MovementSpeed, COND_OwnerOnly);
}

/**
 *  스탯을 초기화 (0으로 설정)
 */
void UR4StatComponent::InitStats()
{
	InitHp();
	InitHpRegenPerSec();
	InitAttackPower();
	InitArmor();
	InitCoolDownReduction();
	InitCriticalChance();
	InitBaseAttackSpeed();
	InitMovementSpeed();
}

/**
 *  주어진 Pk로 스탯 데이터를 채운다.
 */
void UR4StatComponent::PushDTData(FPriKey InPk)
{
	const FR4StatRowPtr statPtr(InPk);
	if(!statPtr.IsValid())
	{
		LOG_ERROR(R4Data, TEXT("StatData is Invalid. PK : [%d]"), InPk);
		return;
	}

	SetBaseHp(statPtr->Hp); SetCurrentHp(Hp.GetBaseValue() + Hp.GetCurrentValue());
	SetBaseHpRegenPerSec(statPtr->HpRegenPerSec);
	SetBaseAttackPower(statPtr->AttackPower);
	SetBaseArmor(statPtr->Armor);
	SetBaseCoolDownReduction(statPtr->CoolDownReduction);
	SetBaseCriticalChance(statPtr->CriticalChance);
	SetBaseBaseAttackSpeed(statPtr->BaseAttackSpeed);
	SetBaseMovementSpeed(statPtr->MovementSpeed);
}

/**
 *  OnRep Funcs
 *  Shadow data와 비교해서 바뀐것만 broadcast
 *  TODO : 테스트 해보쇼
 */
void UR4StatComponent::_OnRep_Hp(const FR4ConsumableStatData& InPrevHp)
{
	R4STAT_CONSUMABLE_STAT_OnRep(Hp, InPrevHp);
}

void UR4StatComponent::_OnRep_HpRegenPerSec(const FR4StatData& InPrevHpRegenPerSec)
{
	R4STAT_STAT_OnRep(HpRegenPerSec, InPrevHpRegenPerSec);
}

void UR4StatComponent::_OnRep_AttackPower(const FR4StatData& InPrevAttackPower)
{
	R4STAT_STAT_OnRep(AttackPower, InPrevAttackPower);
}

void UR4StatComponent::_OnRep_Armor(const FR4StatData& InPrevArmor)
{
	R4STAT_STAT_OnRep(Armor, InPrevArmor);
}

void UR4StatComponent::_OnRep_CoolDownReduction(const FR4StatData& InPrevCoolDownReduction)
{
	R4STAT_STAT_OnRep(CoolDownReduction, InPrevCoolDownReduction);
}

void UR4StatComponent::_OnRep_CriticalChance(const FR4StatData& InPrevCriticalChance)
{
	R4STAT_STAT_OnRep(CriticalChance, InPrevCriticalChance);
}

void UR4StatComponent::_OnRep_BaseAttackSpeed(const FR4StatData& InPrevBaseAttackSpeed)
{
	R4STAT_STAT_OnRep(BaseAttackSpeed, InPrevBaseAttackSpeed);
}

void UR4StatComponent::_OnRep_MovementSpeed(const FR4StatData& InPrevMovementSpeed)
{
	R4STAT_STAT_OnRep(MovementSpeed, InPrevMovementSpeed);
}
