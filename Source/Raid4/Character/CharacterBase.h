// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

class UR4SkillComponent;
class UR4StatComponent;
class UR4SkillBase;
struct FStatRow;

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

private:
	// 스탯을 적용한다.
	void _ApplyTotalStat(const FStatRow& InBaseStat, const FStatRow& InModifierStat);

protected:
	// 스탯 기능을 부여해주는 Stat Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4StatComponent> StatComp;

	// 스킬 기능을 부여해주는 Skill Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillComponent> SkillComp;
};
