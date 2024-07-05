#pragma once

#include "../Data/R4RowBase.h"
#include "R4StatRow.generated.h"

/**
 * 객체의 기본 스탯을 정의할 Row
 */
USTRUCT()
struct FR4StatRow : public FR4RowBase
{
	GENERATED_BODY()

	FR4StatRow() : Hp(0.f), HpRegenPerSec(0.f), Mp(0.f), MpRegenPerSec(0.f), AttackPower(0.f), Armor(0.f), CoolDownReduction(0.f), CriticalChance(0.f), BaseAttackSpeed(0.f), MovementSpeed(0.f) {}
	
	// 기본 체력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float Hp;

	// 기본 초당 체력 재생력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float HpRegenPerSec;
	
	// 기본 마나
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float Mp;

	// 기본 초당 마나 재생력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float MpRegenPerSec;
	
	// 기본 공격력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float AttackPower;

	// 방어력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float Armor;
	
	// 치명타 확률
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float CoolDownReduction;
	
	// 치명타 확률
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float CriticalChance;

	// 이동 속도
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float BaseAttackSpeed;
	
	// 이동 속도
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float MovementSpeed;
};

GENERATE_DT_PTR_H( FR4StatRow );