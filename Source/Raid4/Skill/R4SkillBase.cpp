// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillBase.h"
#include "../Controller/R4PlayerController.h"

#include <GameFramework/Character.h>
#include <Animation/AnimMontage.h>
#include <GameFramework/GameStateBase.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillBase)

UR4SkillBase::UR4SkillBase()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

	CachedLastActivateTime = 0.f;
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
 *  스킬을 사용 ( Main Anim Montage를 Play )
 *  TODO : 버프 걸거 있으면 걸기
 */
void UR4SkillBase::ActivateSkill()
{
    // 내가 플레이 하던거면 애니메이션을 플레이
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()); owner != nullptr && owner->IsLocallyControlled())
	{
		owner->StopAnimMontage();
		owner->PlayAnimMontage(SkillAnim.LoadSynchronous());
	}

    // 서버로 스킬 사용을 알림
	float serverTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
    ServerRPC_ActivateSkill(serverTime);
}

/**
 *  스킬 사용의 유효성을 검증한다.
 *  @param InActivateTime : 클라이언트가 스킬 사용 당시의 서버 시간
 */
bool UR4SkillBase::ServerRPC_ActivateSkill_Validate(float InActivateTime)
{
	// 사용한 시간이 쿨타임 + 오차허용시간 보다 짧다?
	// if((InActivateTime - LastActivateTime) <  (/* 쿹타임 - */ Validation::G_AcceptMinCoolTime) )
	// 	return false;
	return true;
}

/**
 *  서버로 스킬 사용을 알린다.
 *  @param InActivateTime : 클라이언트에서 스킬을 사용했을 때의 서버 시간
 */
void UR4SkillBase::ServerRPC_ActivateSkill_Implementation(float InActivateTime)
{
	// 스킬 사용시간 기록
	CachedLastActivateTime = InActivateTime;

	// TODO : AIController 일때를 대비해서 Interface로 빼면 참 좋겠다 그지?
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
	{
		if(AR4PlayerController* controller = Cast<AR4PlayerController>(owner->GetController()))
		{
			// 클라에서 바로 요청해도 되긴 하는데 Validate 체크를 위해 server에서 호출
			controller->ServerRPC_NotifyPlayAnimMontage(owner, SkillAnim);
		}
	}
}