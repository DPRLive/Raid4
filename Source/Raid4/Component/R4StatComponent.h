// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/DataTable/Row/StatRow.h"
#include "../Stat/R4StatData.h"
#include "../Stat/R4StatMacro.h"
#include <Components/ActorComponent.h>
#include "R4StatComponent.generated.h"

/**
 * 객체에게 스탯 기능을 추가하는 컴포넌트
 * TODO : OnRep 관련 기능 추가
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4StatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UR4StatComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;

protected:
	// BeginPlay
	virtual void BeginPlay() override;

	// Replicate 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Accessors
	R4STAT_CONSUMABLE_STAT_ACCESSORS( Hp );
	R4STAT_STAT_ACCESSORS( HpRegenPerSec );
	R4STAT_CONSUMABLE_STAT_ACCESSORS( Mp );
	R4STAT_STAT_ACCESSORS( MpRegenPerSec );
	R4STAT_STAT_ACCESSORS( AttackPower );
	R4STAT_STAT_ACCESSORS( Armor );
	R4STAT_STAT_ACCESSORS( CoolDownReduction );
	R4STAT_STAT_ACCESSORS( CriticalChance );
	R4STAT_STAT_ACCESSORS( BaseAttackSpeed );
	R4STAT_STAT_ACCESSORS( MovementSpeed );
	
private:
	// HP (체력)
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData Hp;

	// 초당 체력 재생력
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData HpRegenPerSec;
	
	// MP (마나)
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData Mp;

	// 초당 마나 재생력
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData MpRegenPerSec;
	
	// 공격력
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData AttackPower;

	// 방어력
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData Armor;

	// 스킬 쿨타임 감소
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData CoolDownReduction;

	// 치명타 확률
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData CriticalChance;

	// 기본 공격 속도
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData BaseAttackSpeed;

	// 이동 속도
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData MovementSpeed;
};
