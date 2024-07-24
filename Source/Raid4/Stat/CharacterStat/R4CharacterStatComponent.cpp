// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterStatComponent.h"
#include "R4CharacterStatRow.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterStatComponent)

UR4CharacterStatComponent::UR4CharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

/**
 *	컴포넌트 초기화
 */
void UR4CharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);
}

/**
 *  begin play
 */
void UR4CharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Replicate 설정
 */
void UR4CharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate all players
	DOREPLIFETIME(UR4CharacterStatComponent, Hp);
	
	// Replicate Only Owner
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, HpRegenPerSec, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, AttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, Armor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, CoolDownReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, CriticalChance, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, BaseAttackSpeed, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, MovementSpeed, COND_OwnerOnly);
}

/**
 *  Stat을 재설정 (Current Value = Base Value, Modifier Value = 0.f 로 밀어버림)
 */
void UR4CharacterStatComponent::ResetStat()
{
	Super::ResetStat();

	// 데이터 설정은 서버에서
	if(GetOwnerRole() == ROLE_Authority)
	{
		SetModifierHp(0.f); SetCurrentHp(Hp.GetBaseValue());
		SetModifierHpRegenPerSec(0.f); 
		SetModifierAttackPower(0.f); 
		SetModifierArmor(0.f);
		SetModifierCoolDownReduction(0.f);
		SetModifierCriticalChance(0.f);
		SetModifierBaseAttackSpeed(0.f);
		SetModifierMovementSpeed(0.f);
	}
}

/**
 *  주어진 Pk로 스탯 데이터를 채운다.
 */
void UR4CharacterStatComponent::PushDTData(FPriKey InPk)
{
	const FR4CharacterStatRowPtr statPtr(InPk);
	if(!statPtr.IsValid())
	{
		LOG_ERROR(R4Data, TEXT("StatData is Invalid. PK : [%d]"), InPk);
		return;
	}

	// 데이터 설정은 서버에서
	if(GetOwnerRole() == ROLE_Authority)
	{
		SetBaseHp(statPtr->Hp.Value); SetCurrentHp(Hp.GetBaseValue() + Hp.GetCurrentValue());
		SetBaseHpRegenPerSec(statPtr->HpRegenPerSec.Value); 
		SetBaseAttackPower(statPtr->AttackPower.Value); 
		SetBaseArmor(statPtr->Armor.Value);
		SetBaseCoolDownReduction(statPtr->CoolDownReduction.Value);
		SetBaseCriticalChance(statPtr->CriticalChance.Value);
		SetBaseBaseAttackSpeed(statPtr->BaseAttackSpeed.Value);
		SetBaseMovementSpeed(statPtr->MovementSpeed.Value);
	}
	
	// Bind Tag and stat
	ClearTagStats();
	BindTagToStat(statPtr->Hp.Tag, Hp);
	BindTagToStat(statPtr->HpRegenPerSec.Tag, HpRegenPerSec);
	BindTagToStat(statPtr->AttackPower.Tag, AttackPower);
	BindTagToStat(statPtr->Armor.Tag, Armor);
	BindTagToStat(statPtr->CoolDownReduction.Tag, CoolDownReduction);
	BindTagToStat(statPtr->CriticalChance.Tag, CriticalChance);
	BindTagToStat(statPtr->BaseAttackSpeed.Tag, BaseAttackSpeed);
	BindTagToStat(statPtr->MovementSpeed.Tag, MovementSpeed);
}

/**
 *  OnRep Funcs
 *  Shadow data와 비교해서 바뀐것만 broadcast
 *  TODO : 테스트 해보쇼
 */
void UR4CharacterStatComponent::_OnRep_Hp(const FR4ConsumableStatData& InPrevHp)
{
	R4STAT_CONSUMABLE_STAT_OnRep(Hp, InPrevHp);
}

void UR4CharacterStatComponent::_OnRep_HpRegenPerSec(const FR4StatData& InPrevHpRegenPerSec)
{
	R4STAT_STAT_OnRep(HpRegenPerSec, InPrevHpRegenPerSec);
}

void UR4CharacterStatComponent::_OnRep_AttackPower(const FR4StatData& InPrevAttackPower)
{
	R4STAT_STAT_OnRep(AttackPower, InPrevAttackPower);
}

void UR4CharacterStatComponent::_OnRep_Armor(const FR4StatData& InPrevArmor)
{
	R4STAT_STAT_OnRep(Armor, InPrevArmor);
}

void UR4CharacterStatComponent::_OnRep_CoolDownReduction(const FR4StatData& InPrevCoolDownReduction)
{
	R4STAT_STAT_OnRep(CoolDownReduction, InPrevCoolDownReduction);
}

void UR4CharacterStatComponent::_OnRep_CriticalChance(const FR4StatData& InPrevCriticalChance)
{
	R4STAT_STAT_OnRep(CriticalChance, InPrevCriticalChance);
}

void UR4CharacterStatComponent::_OnRep_BaseAttackSpeed(const FR4StatData& InPrevBaseAttackSpeed)
{
	R4STAT_STAT_OnRep(BaseAttackSpeed, InPrevBaseAttackSpeed);
}

void UR4CharacterStatComponent::_OnRep_MovementSpeed(const FR4StatData& InPrevMovementSpeed)
{
	R4STAT_STAT_OnRep(MovementSpeed, InPrevMovementSpeed);
}
