// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interface/R4PlayerInputable.h"
#include "R4SkillBase.h"
#include "R4PlayerSkillNormal.generated.h"

/**
 * 플레이어 일반 입력 스킬 ( 누르면 발동하는 거 )
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAID4_API UR4PlayerSkillNormal : public UR4SkillBase, public IR4PlayerInputable
{
	GENERATED_BODY()

public:
	UR4PlayerSkillNormal();
	
	// 스킬 입력 시작
	virtual void OnInputStarted() override;

	// 스킬 입력 중
	virtual void OnInputTriggered() override;

	// 스킬 입력 종료
	virtual void OnInputCompleted() override;

private:
	// 발동할 스킬 애니메이션, 누르면 바로 발동한다.
	UPROPERTY( EditAnywhere, Category = "Skill|Anim" )
	FSkillAnimInfo SkillAnimInfo;
};