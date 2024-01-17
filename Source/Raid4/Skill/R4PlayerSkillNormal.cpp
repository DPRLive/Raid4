// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillNormal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerSkillNormal)

UR4PlayerSkillNormal::UR4PlayerSkillNormal()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

/**
 *  스킬 입력 시작
 *  Normal 스킬은 시작할 때 한 번 Activate함.
 */
void UR4PlayerSkillNormal::OnInputSkillStarted()
{
    ActivateSkill();
}

/**
 *  스킬 입력 중
 */
void UR4PlayerSkillNormal::OnInputSkillTriggered()
{
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerSkillNormal::OnInputSkillCompleted()
{
}
