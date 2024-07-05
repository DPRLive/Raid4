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
 */
void UR4PlayerSkillNormal::OnInputStarted()
{
    PlaySkillAnim(SkillAnimInfo);
}

/**
 *  스킬 입력 중
 */
void UR4PlayerSkillNormal::OnInputTriggered()
{
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerSkillNormal::OnInputCompleted()
{
}
