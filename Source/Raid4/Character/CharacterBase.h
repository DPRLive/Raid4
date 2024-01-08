// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interface/R4SkillInterface.h"
#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

class UR4SkillManageComponent;

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API ACharacterBase : public ACharacter, public IR4SkillInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& InObjectInitializer);
	
protected:
	virtual void BeginPlay() override;

public:
	// 현재 캐릭터가 스킬을 사용할 수 있는지 판단
	virtual bool CanActivateSkill() override;
	
private:
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillManageComponent> SkillManageComp;
};
