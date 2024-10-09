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
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, MovementSpeed, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, ApplyDamageMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4CharacterStatComponent, ReceiveDamageMultiplier, COND_OwnerOnly);
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
		SetAddModifierHp(0.f); SetMultiplyModifierHp(1.f); SetCurrentHp(Hp.GetTotalValue());
		SetAddModifierHpRegenPerSec(0.f); SetMultiplyModifierHpRegenPerSec(1.f);
		SetAddModifierAttackPower(0.f); SetMultiplyModifierAttackPower(1.f);
		SetAddModifierArmor(0.f); SetMultiplyModifierArmor(1.f);
		SetAddModifierCoolDownReduction(0.f); SetMultiplyModifierCoolDownReduction(1.f);
		SetAddModifierCriticalChance(0.f); SetMultiplyModifierCriticalChance(1.f);
		SetAddModifierMovementSpeed(0.f); SetMultiplyModifierMovementSpeed(1.f);
		SetAddModifierApplyDamageMultiplier(0.f); SetMultiplyModifierApplyDamageMultiplier(1.f);
		SetAddModifierReceiveDamageMultiplier(0.f); SetMultiplyModifierReceiveDamageMultiplier(1.f);
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
		SetBaseHp(statPtr->Hp.Value); SetCurrentHp(Hp.GetTotalValue());
		SetBaseHpRegenPerSec(statPtr->HpRegenPerSec.Value); 
		SetBaseAttackPower(statPtr->AttackPower.Value); 
		SetBaseArmor(statPtr->Armor.Value);
		SetBaseCoolDownReduction(statPtr->CoolDownReduction.Value);
		SetBaseCriticalChance(statPtr->CriticalChance.Value);
		SetBaseMovementSpeed(statPtr->MovementSpeed.Value);

		// Meta Stat
		SetBaseApplyDamageMultiplier(1.f);
		SetBaseReceiveDamageMultiplier(1.f);
	}
	
	// Bind Tag and stat
	ClearTagStats();
	BindTagToStat(statPtr->Hp.Tag, Hp);
	BindTagToStat(statPtr->HpRegenPerSec.Tag, HpRegenPerSec);
	BindTagToStat(statPtr->AttackPower.Tag, AttackPower);
	BindTagToStat(statPtr->Armor.Tag, Armor);
	BindTagToStat(statPtr->CoolDownReduction.Tag, CoolDownReduction);
	BindTagToStat(statPtr->CriticalChance.Tag, CriticalChance);
	BindTagToStat(statPtr->MovementSpeed.Tag, MovementSpeed);

	// Meta Stat
	BindTagToStat(TAG_STAT_NORMAL_ApplyDamageMultiplier, ApplyDamageMultiplier);
	BindTagToStat(TAG_STAT_NORMAL_ReceiveDamageMultiplier, ReceiveDamageMultiplier);
}

/**
 *  OnRep Funcs
 *  Shadow data와 비교해서 바뀐것만 broadcast
 */
void UR4CharacterStatComponent::_OnRep_Hp(const FR4CurrentStatInfo& InPrevHp)
{
	R4STAT_CURRENT_STAT_OnRep(Hp, InPrevHp);
}

void UR4CharacterStatComponent::_OnRep_HpRegenPerSec(const FR4StatInfo& InPrevHpRegenPerSec)
{
	R4STAT_STAT_OnRep(HpRegenPerSec, InPrevHpRegenPerSec);
}

void UR4CharacterStatComponent::_OnRep_AttackPower(const FR4StatInfo& InPrevAttackPower)
{
	R4STAT_STAT_OnRep(AttackPower, InPrevAttackPower);
}

void UR4CharacterStatComponent::_OnRep_Armor(const FR4StatInfo& InPrevArmor)
{
	R4STAT_STAT_OnRep(Armor, InPrevArmor);
}

void UR4CharacterStatComponent::_OnRep_CoolDownReduction(const FR4StatInfo& InPrevCoolDownReduction)
{
	R4STAT_STAT_OnRep(CoolDownReduction, InPrevCoolDownReduction);
}

void UR4CharacterStatComponent::_OnRep_CriticalChance(const FR4StatInfo& InPrevCriticalChance)
{
	R4STAT_STAT_OnRep(CriticalChance, InPrevCriticalChance);
}

void UR4CharacterStatComponent::_OnRep_MovementSpeed(const FR4StatInfo& InPrevMovementSpeed)
{
	R4STAT_STAT_OnRep(MovementSpeed, InPrevMovementSpeed);
}

void UR4CharacterStatComponent::_OnRep_ApplyDamageMultiplier(const FR4StatInfo& InApplyDamageMultiplier)
{
	R4STAT_STAT_OnRep(ApplyDamageMultiplier, InApplyDamageMultiplier);
}

void UR4CharacterStatComponent::_OnRep_ReceiveDamageMultiplier(const FR4StatInfo& InReceiveDamageMultiplier)
{
	R4STAT_STAT_OnRep(ReceiveDamageMultiplier, InReceiveDamageMultiplier);
}
