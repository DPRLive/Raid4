// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Input/R4PlayerInputableInterface.h"

#include "../R4AnimSkillBase.h"

#include "R4Skill_PlayerNormal.generated.h"

/**
 * 플레이어 일반 입력 스킬 ( 누르면 발동 )
 */
UCLASS( Blueprintable, NotBlueprintType, ClassGroup=(Skill))
class RAID4_API UR4Skill_PlayerNormal : public UR4AnimSkillBase, public IR4PlayerInputableInterface
{
	GENERATED_BODY()

public:
	UR4Skill_PlayerNormal();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// ~ Begin IR4PlayerInputableInterface
	virtual void OnInputStarted() override;
	virtual void OnInputTriggered() override {}
	virtual void OnInputCompleted() override {}
	// ~ End IR4PlayerInputableInterface

protected:
	// Anim을 Play시작 시 호출.
	virtual void OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, float InStartServerTime ) override;

private:
	// 발동할 스킬 애니메이션, 누르면 바로 작동
	UPROPERTY( Replicated, EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo NormalSkillAnimInfo;
};