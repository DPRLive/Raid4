// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4StatStruct.h"
#include "../R4StatMacro.h"
#include "../../Data/R4DTDataPushInterface.h"
#include "../R4TagStatBaseComponent.h"
#include "R4CharacterStatComponent.generated.h"

/**
 * 캐릭터에게 스탯 기능을 추가하는 컴포넌트
 */
UCLASS( ClassGroup=(Stat), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4CharacterStatComponent : public UR4TagStatBaseComponent, public IR4DTDataPushInterface
{
	GENERATED_BODY()
	
public:	
	UR4CharacterStatComponent();

protected:
	// Replicate 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//  Stat을 초기화 (Current Value = Base Value, Modifier Value = 0.f, Delegate Clear )
	virtual void Clear() override;
	
	// ~ Begin IR4DTDataPushable (주어진 Pk로 스탯 데이터를 채운다. ( By DT_Stat))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable
	
public:
	// Accessors
	R4STAT_CURRENT_STAT_ACCESSORS( Hp );
	R4STAT_STAT_ACCESSORS( HpRegenPerSec );
	R4STAT_STAT_ACCESSORS( AttackPower );
	R4STAT_STAT_ACCESSORS( Armor );
	R4STAT_STAT_ACCESSORS( CoolDownReduction );
	R4STAT_STAT_ACCESSORS( CriticalChance );
	R4STAT_STAT_ACCESSORS( MovementSpeed );
	R4STAT_STAT_ACCESSORS( ApplyDamageMultiplier );
	R4STAT_STAT_ACCESSORS( ReceiveDamageMultiplier );
	
private:
	// HP (체력)
	UPROPERTY( ReplicatedUsing = _OnRep_Hp, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4CurrentStatInfo Hp;

	// 초당 체력 재생력
	UPROPERTY( ReplicatedUsing = _OnRep_HpRegenPerSec, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo HpRegenPerSec;
	
	// 공격력
	UPROPERTY( ReplicatedUsing = _OnRep_AttackPower, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo AttackPower;

	// 방어력
	UPROPERTY( ReplicatedUsing = _OnRep_Armor, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo Armor;

	// 스킬 쿨타임 감소 ( 백분율 )
	UPROPERTY( ReplicatedUsing = _OnRep_CoolDownReduction, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo CoolDownReduction;

	// 치명타 확률 ( 백분율 )
	UPROPERTY( ReplicatedUsing = _OnRep_CriticalChance, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo CriticalChance;

	// 이동 속도
	UPROPERTY( ReplicatedUsing = _OnRep_MovementSpeed, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo MovementSpeed;

	// Meta Stat //
	
	// 주는 피해량 증감량 (비율)
	UPROPERTY( ReplicatedUsing = _OnRep_ApplyDamageMultiplier, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo ApplyDamageMultiplier;
	
	// 받는 피해량 증감량 (비율)
	UPROPERTY( ReplicatedUsing = _OnRep_ReceiveDamageMultiplier, VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = true) )
	FR4StatInfo ReceiveDamageMultiplier;
	
private:
	// OnRep 함수들
	UFUNCTION()
	void _OnRep_Hp(const FR4CurrentStatInfo& InPrevHp);
	
	UFUNCTION()
	void _OnRep_HpRegenPerSec(const FR4StatInfo& InPrevHpRegenPerSec);

	UFUNCTION()
	void _OnRep_AttackPower(const FR4StatInfo& InPrevAttackPower);

	UFUNCTION()
	void _OnRep_Armor(const FR4StatInfo& InPrevArmor);

	UFUNCTION()
	void _OnRep_CoolDownReduction(const FR4StatInfo& InPrevCoolDownReduction);

	UFUNCTION()
	void _OnRep_CriticalChance(const FR4StatInfo& InPrevCriticalChance);

	UFUNCTION()
	void _OnRep_MovementSpeed(const FR4StatInfo& InPrevMovementSpeed);

	UFUNCTION()
	void _OnRep_ApplyDamageMultiplier(const FR4StatInfo& InApplyDamageMultiplier);

	UFUNCTION()
	void _OnRep_ReceiveDamageMultiplier(const FR4StatInfo& InReceiveDamageMultiplier);
};
