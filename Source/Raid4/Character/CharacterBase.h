// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

class UR4SkillManageComponent;
class UR4StatManageComponent;
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

public:
	// 스킬을 추가
	void AddSkill(const ESkillIndex InSkillIndex, UR4SkillBase* InSkill);

	// 스킬 Map을 반환
	const TMap<ESkillIndex, TObjectPtr<UR4SkillBase>>& GetSkills() const { return InstancedSkills; }
private:
	// 스탯을 적용한다.
	void _ApplyTotalStat(const FStatRow& InBaseStat, const FStatRow& InModifierStat);

	// 스탯을 관리해주는 Stat Manage Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4StatManageComponent> StatManageComp;

protected:
	// 실제로 인스턴스화 된 스킬
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Skill" )
	TMap<ESkillIndex, TObjectPtr<UR4SkillBase>> InstancedSkills;

	// 스킬을 관리해주는 Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillManageComponent> SkillManageComp;
};
