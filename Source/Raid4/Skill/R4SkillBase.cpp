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
    // 자율 프록시면 애니메이션을 플레이
	if(GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		_PlaySkillAnim();
	}

	// 서버로 스킬 사용을 알림
	ServerRPC_ActivateSkill();
}

/**
 *  스킬이 끝나기 전에 Cancel, Montage를 스탑
 *  TODO : 취소를 위한 로직 추가
 */
void UR4SkillBase::CancelSkill()
{
	
}

/**
 *  스킬 사용 완료
 */
void UR4SkillBase::CompleteSkill()
{
}

/**
 *  서버로 스킬 사용을 알린다.
 */
void UR4SkillBase::ServerRPC_ActivateSkill_Implementation()
{
	// 서버에서 애니메이션을 플레이 후
    _PlaySkillAnim();

    // 클라이언트들에게 스킬 사용을 명령한다. ( 시뮬레이트 프록시들에게 )
    MulticastRPC_ActivateSkill();
}

/**
 *  클라이언트들에게 스킬 사용을 명령한다.
 */
void UR4SkillBase::MulticastRPC_ActivateSkill_Implementation()
{
    // 시뮬레이티드면 애니메이션을 플레이
    if(GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
    {
        _PlaySkillAnim();
    }

	// ... TODO
}

/**
 *  스킬 Anim Montage를 플레이
 */
void UR4SkillBase::_PlaySkillAnim()
{
    if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
    {
        owner->PlayAnimMontage(LoadSoftObjectSync(SkillAnim));
    }
}

/**
 *  스킬 Anim Montage를 중지
 */
void UR4SkillBase::_StopSkillAnim()
{
    if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
	{
		owner->StopAnimMontage(LoadSoftObjectSync(SkillAnim));
	}
}

