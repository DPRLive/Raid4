// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerNormal.h"

#include <Net/UnrealNetwork.h>
#include <GameFramework/Pawn.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerNormal)

UR4Skill_PlayerNormal::UR4Skill_PlayerNormal()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UR4Skill_PlayerNormal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION( UR4Skill_PlayerNormal, NormalSkillAnimInfo, COND_OwnerOnly );
}

/**
 *  스킬 입력 시작, 입력시 바로 실행
 */
void UR4Skill_PlayerNormal::OnInputStarted()
{
	// Locally Control인지 확인
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if(!IsValid(ownerPawn) || !(ownerPawn->IsLocallyControlled()))
		return;

	// 스킬 사용 가능 상태인지 확인
	if(!CanActivateSkill())
		return;

	PlaySkillAnim( NormalSkillAnimInfo );
}

/**
 * Anim을 Play시작 시 호출. Key에 맞춰 할 로직 설정. Server와 Owner Client 에서 호출.
 * @param InSkillAnimKey : Server에서 부여받은 Skill Anim의 Key.
 */
void UR4Skill_PlayerNormal::OnBeginSkillAnim( uint32 InSkillAnimKey )
{
	// Normal Skill의 경우 Anim Play 시점을 Skill 사용으로 판정
	// Anim Play = 스킬 사용으로 판단.
	if(InSkillAnimKey == NormalSkillAnimInfo.SkillAnimServerKey)
		SetSkillCoolDownTime( GetSkillCoolDownTime( true ) );
}

/**
 *  Skill Anim Key 값에 맞는 Skill Anim 을 현재 Play할 수 없는지 확인.
 *  Client에서 PlaySkillAnim시에 확인 및
 *  PlayAnim Server RPC에서 Validation Check에 사용
 *  @param InSkillAnimKey : Server에서 부여받은 Skill Anim의 Key.
 */
bool UR4Skill_PlayerNormal::IsLockPlaySkillAnim(uint32 InSkillAnimKey) const
{
	// Normal Skill Anim의 Server Play는 Skill 사용이 가능할 때만 서버에서 허용
	if(InSkillAnimKey == NormalSkillAnimInfo.SkillAnimServerKey)
		return !CanActivateSkill();

	return true;
}