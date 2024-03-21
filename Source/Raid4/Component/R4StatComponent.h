// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	// 스탯을 주어진 Pk로 초기화한다.
	void InitStat(FPriKey InPk);
	
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
	UPROPERTY( ReplicatedUsing = _OnRep_Hp, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData Hp;

	// 초당 체력 재생력
	UPROPERTY( ReplicatedUsing = _OnRep_HpRegenPerSec, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData HpRegenPerSec;
	
	// MP (마나)
	UPROPERTY( ReplicatedUsing = _OnRep_Mp, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4ConsumableStatData Mp;

	// 초당 마나 재생력
	UPROPERTY( ReplicatedUsing = _OnRep_MpRegenPerSec, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData MpRegenPerSec;
	
	// 공격력
	UPROPERTY( ReplicatedUsing = _OnRep_AttackPower, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData AttackPower;

	// 방어력
	UPROPERTY( ReplicatedUsing = _OnRep_Armor, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData Armor;

	// 스킬 쿨타임 감소
	UPROPERTY( ReplicatedUsing = _OnRep_CoolDownReduction, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData CoolDownReduction;

	// 치명타 확률
	UPROPERTY( ReplicatedUsing = _OnRep_CriticalChance, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData CriticalChance;

	// 기본 공격 속도
	UPROPERTY( ReplicatedUsing = _OnRep_BaseAttackSpeed, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData BaseAttackSpeed;

	// 이동 속도
	UPROPERTY( ReplicatedUsing = _OnRep_MovementSpeed, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatData MovementSpeed;

private:
	// OnRep 함수들
	UFUNCTION()
	void _OnRep_Hp(const FR4ConsumableStatData& InPrevHp);
	
	UFUNCTION()
	void _OnRep_HpRegenPerSec(const FR4StatData& InPrevHpRegenPerSec);

	UFUNCTION()
	void _OnRep_Mp(const FR4ConsumableStatData& InPrevMp);

	UFUNCTION()
	void _OnRep_MpRegenPerSec(const FR4StatData& InPrevMpRegenPerSec);

	UFUNCTION()
	void _OnRep_AttackPower(const FR4StatData& InPrevAttackPower);

	UFUNCTION()
	void _OnRep_Armor(const FR4StatData& InPrevArmor);

	UFUNCTION()
	void _OnRep_CoolDownReduction(const FR4StatData& InPrevCoolDownReduction);

	UFUNCTION()
	void _OnRep_CriticalChance(const FR4StatData& InPrevCriticalChance);

	UFUNCTION()
	void _OnRep_BaseAttackSpeed(const FR4StatData& InPrevBaseAttackSpeed);

	UFUNCTION()
	void _OnRep_MovementSpeed(const FR4StatData& InPrevMovementSpeed);
};
