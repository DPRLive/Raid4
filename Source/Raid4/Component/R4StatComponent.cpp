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
 *  TODO : Replicate 설정
 */
void UR4StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/**
 *  스탯을 주어진 Pk로 초기화한다.
 */
void UR4StatComponent::InitStat(FPriKey InPk)
{
	const FStatRowPtr statPtr(InPk);
	if(!statPtr.IsValid())
		return;

	InitHp(statPtr->Hp);
	InitHpRegenPerSec(statPtr->HpRegenPerSec);
	InitMp(statPtr->Mp);
	InitMpRegenPerSec(statPtr->MpRegenPerSec);
	InitAttackPower(statPtr->MpRegenPerSec);
	InitArmor(statPtr->Armor);
	InitCoolDownReduction(statPtr->CoolDownReduction);
	InitCriticalChance(statPtr->CriticalChance);
	InitBaseAttackSpeed(statPtr->BaseAttackSpeed);
	InitMovementSpeed(statPtr->MovementSpeed);
}