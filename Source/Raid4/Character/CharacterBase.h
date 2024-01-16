// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interface/R4SkillInterface.h"
#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

class UR4SkillManageComponent;
class UR4StatManageComponent;
struct FStatRow;

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API ACharacterBase : public ACharacter, public IR4SkillInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// 현재 캐릭터가 스킬을 사용할 수 있는지 판단
	virtual bool CanActivateSkill() override;
	
private:
	// 스탯을 적용한다.
	void _ApplyTotalStat(const FStatRow& InBaseStat, const FStatRow& InModifierStat);
	
	// 스킬을 관리 해주는 Skill Manage Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillManageComponent> SkillManageComp;

	// 스탯을 관리해주는 Stat Manage Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4StatManageComponent> StatManageComp;
};
