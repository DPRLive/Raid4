// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4SkillComponent.h"
#include "R4PlayerSkillComponent.generated.h"

/**
 * Player용 Skill Component
 */
UCLASS( ClassGroup=(Skill), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4PlayerSkillComponent : public UR4SkillComponent
{
	GENERATED_BODY()

public:
	UR4PlayerSkillComponent();

public:
	// 스킬 입력 처리
	virtual void OnInputSkillStarted( EPlayerSkillIndex InSkillIndex );
	virtual void OnInputSkillTriggered( EPlayerSkillIndex InSkillIndex );
	virtual void OnInputSkillCompleted( EPlayerSkillIndex InSkillIndex );

	virtual void Clear();
protected:
	// 스킬이 등록 된 후 호출.
	virtual void PostAddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill ) override;

	// 스킬이 등록 해제 되기 전 호출
	virtual void PreRemoveSkill( uint8 InSkillIndex, UR4SkillBase* InSkill ) override;

private:
	// 스킬 사용 제한을 위한 Delegate Handle {OnBegin, OnEnd}
	TArray<TPair<FDelegateHandle, FDelegateHandle>> SkillDelegateHandles;

};
