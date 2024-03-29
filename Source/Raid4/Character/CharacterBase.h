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

protected:
	// 주어진 Character Data PK로 데이터를 읽어 초기화한다.
	virtual void InitCharacterData(FPriKey InCharacterDataPk);
	
	// 주어진 스탯 Data로 스탯을 초기화한다.
	virtual void InitStatData(FPriKey InStatPk);
	
	// 이동 속도를 적용한다.
	virtual void ApplyMovementSpeed(float InBaseMovementSpeed, float InModifierMovementSpeed) const;

protected:
	// 스탯 기능을 부여해주는 Stat Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4StatComponent> StatComp;

	// 스킬 기능을 부여해주는 Skill Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillComponent> SkillComp;
};
