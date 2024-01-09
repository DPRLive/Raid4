#pragma once

#include "../R4RowBase.h"
#include "StatRow.generated.h"

/**
 * 객체의 기본 스탯을 정의할 Row
 */
USTRUCT()
struct FStatRow : public FR4RowBase
{
	GENERATED_BODY()

	FStatRow() : MaxHp(0.f), MaxMp(0.f), AttackDamage(0.f), CriticalRate(0.f), Defensive(0.f), MovementSpeed(0.f) {}
	
	// 최대 체력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float MaxHp;

	// 최대 마나
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float MaxMp;

	// 기본 공격력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float AttackDamage;

	// 크리티컬 확률
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float CriticalRate;
	
	// 방어력
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float Defensive;
	
	// 이동 속도
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	float MovementSpeed;

	FStatRow& operator+=(const FStatRow& InRow)
	{
		MaxHp += InRow.MaxHp;
		MaxMp += InRow.MaxMp;
		AttackDamage += InRow.AttackDamage;
		CriticalRate += InRow.CriticalRate;
		Defensive += InRow.Defensive;
		MovementSpeed += InRow.MovementSpeed;
	
		return *this;
	}
};

inline FStatRow operator+(const FStatRow& InRowL, const FStatRow& InRowR)
{
	FStatRow ret;
	ret.MaxHp = InRowL.MaxHp + InRowR.MaxHp;
	ret.MaxMp = InRowL.MaxMp + InRowR.MaxMp;
	ret.AttackDamage = InRowL.AttackDamage + InRowR.AttackDamage;
	ret.CriticalRate = InRowL.CriticalRate + InRowR.CriticalRate;
	ret.Defensive = InRowL.Defensive + InRowR.Defensive;
	ret.MovementSpeed = InRowL.MovementSpeed + InRowR.MovementSpeed;
	
	return ret;
}

GENERATE_DT_PTR_H( FStatRow );