// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerNormal.h"

#include <Net/UnrealNetwork.h>

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
	// 스킬 사용 가능 상태인지 확인
	if(!CanActivateSkill())
		return;

	PlaySkillAnim( NormalSkillAnimInfo );
}

/**
 * Anim을 Play시작 시 호출. Server와 Owner Client 에서 호출.
 * @param InInstanceID : 부여된 MontageInstanceID
 * @param InSkillAnimInfo : Play될 Skill Anim 정보
 */
void UR4Skill_PlayerNormal::OnBeginSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo )
{
	Super::OnBeginSkillAnim( InInstanceID , InSkillAnimInfo );
	
	// Normal Skill의 경우 Anim Play 시점을 Skill 사용으로 판정
	// Anim Play 성공 = 스킬 사용으로 판단 및 쿨타임 적용
	if ( InSkillAnimInfo.SkillAnimServerKey == NormalSkillAnimInfo.SkillAnimServerKey )
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
}

/**
 *  Server RPC의 Play Skill Anim 시 요청 무시 check에 사용.
 * (Validate Check 후 Server RPC 내에서 체크함으로 Index가 유효함이 보장)
 */
bool UR4Skill_PlayerNormal::PlaySkillAnim_Ignore( uint32 InSkillAnimKey ) const
{
	// Skill을 사용할 수 없는 상태인데 요청이 오는 경우 무시
	if(InSkillAnimKey == NormalSkillAnimInfo.SkillAnimServerKey)
		return Super::PlaySkillAnim_Ignore( InSkillAnimKey ) || !CanActivateSkill();

	return true;
}