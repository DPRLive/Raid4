// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interface/R4PlayerSkillInterface.h"
#include "R4SkillBase.h"
#include "R4PlayerSkillNormal.generated.h"

/**
 * 플레이어 일반 입력 스킬 ( 누르면 발동하는 거 )
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4PlayerSkillNormal : public UR4SkillBase, public IR4PlayerSkillInterface
{
	GENERATED_BODY()

public:
	UR4PlayerSkillNormal();
	
	// 스킬 입력 시작
	virtual void OnInputSkillStarted() override;

	// 스킬 입력 중
	virtual void OnInputSkillTriggered() override;

	// 스킬 입력 종료
	virtual void OnInputSkillCompleted() override;
};