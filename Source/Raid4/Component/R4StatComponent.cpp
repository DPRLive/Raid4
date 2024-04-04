// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatComponent.h"
#include "../Data/DataTable/Row/StatRow.h"

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
	DOREPLIFETIME(UR4StatComponent, Mp);
	
	// Replicate Only Owner
	DOREPLIFETIME_CONDITION(UR4StatComponent, HpRegenPerSec, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, MpRegenPerSec, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, AttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, Armor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, CoolDownReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, CriticalChance, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, BaseAttackSpeed, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatComponent, MovementSpeed, COND_OwnerOnly);
}

/**
 *  스탯을 주어진 Pk로 초기화한다.
 */
void UR4StatComponent::InitializeByDTPriKey(FPriKey InPk)
{
	const FStatRowPtr statPtr(InPk);
	if(!statPtr.IsValid())
	{
		LOG_ERROR(R4Data, TEXT("StatData is Invalid. PK : [%d]"), InPk);
		return;
	}

	InitHp(statPtr->Hp);
	InitHpRegenPerSec(statPtr->HpRegenPerSec);
	InitMp(statPtr->Mp);
	InitMpRegenPerSec(statPtr->MpRegenPerSec);
	InitAttackPower(statPtr->AttackPower);
	InitArmor(statPtr->Armor);
	InitCoolDownReduction(statPtr->CoolDownReduction);
	InitCriticalChance(statPtr->CriticalChance);
	InitBaseAttackSpeed(statPtr->BaseAttackSpeed);
	InitMovementSpeed(statPtr->MovementSpeed);
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

void UR4StatComponent::_OnRep_Mp(const FR4ConsumableStatData& InPrevMp)
{
	R4STAT_CONSUMABLE_STAT_OnRep(Mp, InPrevMp);
}

void UR4StatComponent::_OnRep_MpRegenPerSec(const FR4StatData& InPrevMpRegenPerSec)
{
	R4STAT_STAT_OnRep(MpRegenPerSec, InPrevMpRegenPerSec);
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
