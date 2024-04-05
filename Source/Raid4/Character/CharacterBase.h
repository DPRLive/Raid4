// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Interface\R4DTDataPushable.h"

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
class RAID4_API ACharacterBase : public ACharacter, public IR4DTDataPushable
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// ~ Begin IR4DTDataPushable (Character의 데이터를 초기화한다. ( By DT_Character))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable
	
protected:
	// StatComp와 필요한 초기화를 진행한다
	virtual void InitStatComponent(FPriKey InStatPk);
	
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
