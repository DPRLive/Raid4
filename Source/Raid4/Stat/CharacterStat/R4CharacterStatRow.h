#pragma once

#include "../../Data/R4RowBase.h"
#include "R4CharacterStatRow.generated.h"

/**
 * 객체의 기본 스탯을 정의할 Row
 */
USTRUCT()
struct FR4CharacterStatRow : public FR4RowBase
{
	GENERATED_BODY()

	FR4CharacterStatRow()
	: Hp({TAG_STAT_CURRENT_Hp, 0.f})
	, HpRegenPerSec({TAG_STAT_NORMAL_HpRegenPerSec, 0.f})
	, AttackPower({TAG_STAT_NORMAL_AttackPower, 0.f})
	, Armor({TAG_STAT_NORMAL_Armor, 0.f})
	, CoolDownReduction({TAG_STAT_NORMAL_CoolDownReduction, 0.f})
	, CriticalChance({TAG_STAT_NORMAL_CriticalChance, 0.f})
	, BaseAttackSpeed({TAG_STAT_NORMAL_BaseAttackSpeed, 0.f})
	, MovementSpeed({TAG_STAT_NORMAL_MovementSpeed, 0.f}) {}
	
	// 기본 체력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue Hp;

	// 기본 초당 체력 재생력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue HpRegenPerSec;
	
	// 기본 공격력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue AttackPower;

	// 방어력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue Armor;
	
	// 쿨타임 감소 ( 백분율 )
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue CoolDownReduction;
	
	// 치명타 확률 ( 백분율 )
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue CriticalChance;

	// 공격 속도
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue BaseAttackSpeed;
	
	// 이동 속도
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	FR4TagValue MovementSpeed;
};

GENERATE_DT_PTR_H( FR4CharacterStatRow );