// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4SkillComponent.h"
#include "R4NonPlayerSkillComponent.generated.h"

/**
 * NPC용 Skill Component
 */
UCLASS( ClassGroup=(Skill), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4NonPlayerSkillComponent : public UR4SkillComponent
{
	GENERATED_BODY()

public:
	// 스킬 사용 요청 처리
	void ActivateAISkill( uint8 InSkillIndex );

	// 사용 가능한 스킬 중, ActivateSkillMinDist가 가장 큰 Skill Index를 반환
	int32 GetAvailableMaxDistSkillIndex( float& OutDist ) const;
};
