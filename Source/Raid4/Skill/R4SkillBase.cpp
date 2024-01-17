// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillBase.h"

#include <GameFramework/Character.h>
#include <Animation/AnimMontage.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillBase)

UR4SkillBase::UR4SkillBase()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  스킬을 준비
 */
void UR4SkillBase::PrepareSkill()
{
}

/**
 *  스킬을 사용 ( Main Anim Montage를 Play )
 *  TODO : 버프 걸거 있으면 걸기
 */
void UR4SkillBase::ActivateSkill()
{
    if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
    {
        owner->PlayAnimMontage(LoadSoftObjectSync(SkillMainAnim));
    }
}

/**
 *  스킬이 끝나기 전에 Cancel, Montage를 스탑
 */
void UR4SkillBase::CancelSkill()
{
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
	{
		owner->StopAnimMontage(LoadSoftObjectSync(SkillMainAnim));
	}
}

/**
 *  스킬 사용 완료
 */
void UR4SkillBase::CompleteSkill()
{
}

