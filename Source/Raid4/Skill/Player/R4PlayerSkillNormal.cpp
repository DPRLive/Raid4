// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillNormal.h"
#include "../../Animation/R4AnimationInterface.h"

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
    //ServerRPC_PlayAnim();

	if(GetOwnerRole() != ROLE_Authority)
	{
		if(IR4AnimationInterface* owner = Cast<IR4AnimationInterface>(GetOwner()))
		{
			owner->PlayAnim_Local(SkillAnimInfo.SkillAnim, NAME_None, 1.f);
		}
	}
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

// void UR4PlayerSkillNormal::ServerRPC_PlayAnim_Implementation()
// {
// 	if(IR4AnimationInterface* owner = Cast<IR4AnimationInterface>(GetOwner()))
// 	{
// 		owner->Server_PlayAnim_WithoutAutonomous(SkillAnimInfo.SkillAnim, NAME_None, 1.f, true, R4GetServerTimeSeconds());
// 	}
// }
