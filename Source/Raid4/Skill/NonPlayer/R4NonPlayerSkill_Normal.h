// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4NonPlayerSkillInterface.h"

#include <Raid4/Skill/R4AnimSkillBase.h>

#include "R4NonPlayerSkill_Normal.generated.h"

/**
 * NPC 일반 스킬.
 */
UCLASS( Blueprintable, NotBlueprintType, ClassGroup=(Skill))
class RAID4_API UR4NonPlayerSkill_Normal : public UR4AnimSkillBase, public IR4NonPlayerSkillInterface
{
	GENERATED_BODY()

public:
	UR4NonPlayerSkill_Normal();

protected:
	// ~ Begin IR4NonPlayerSkillInterface
	virtual void ActivateAISkill() override;
	virtual float GetActivateSkillMinDist() const override { return ActivateSkillMinDist; }
	// ~ End IR4NonPlayerSkillInterface
	
	// Anim을 Play시작 시 호출.
	virtual void OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, float InStartServerTime ) override;

	// Anim 종료 시 호출.
	virtual void OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted ) override;

private:
	// 발동할 스킬 애니메이션
	UPROPERTY( EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo SkillAnimInfo;

	// AI가 Skill을 사용 시, Target과의 최소 거리.
	UPROPERTY( EditAnywhere, Category = "Skill" )
	float ActivateSkillMinDist;
};
