// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerNormal.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerNormal)

UR4Skill_PlayerNormal::UR4Skill_PlayerNormal()
{
	SetIsReplicatedByDefault( true );
}

void UR4Skill_PlayerNormal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Anim Key를 Replicate 받을 수 있도록 설정
	DOREPLIFETIME_CONDITION( UR4Skill_PlayerNormal, NormalSkillAnimInfo, COND_InitialOnly );
}

/**
 *  스킬 입력 시작, 입력시 바로 실행
 */
void UR4Skill_PlayerNormal::OnInputStarted()
{
	// 스킬 사용 가능 상태인지 확인
	if ( !CanActivateSkill() )
		return;

	PlaySkillAnim_WithServerRPC( NormalSkillAnimInfo, NAME_None );
}

/**
 * Anim을 Play시작 시 호출
 * @param InSkillAnimInfo : Play된 Skill Anim 정보
 * @param InStartSectionName : 시작 된 Section Name.
 * @param InStartServerTime : Skill Anim이 시작된 Server Time
 */
void UR4Skill_PlayerNormal::OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	Super::OnBeginSkillAnim( InSkillAnimInfo, InStartSectionName, InStartServerTime );
	
	// Normal Skill의 경우 Anim Play 시점을 Skill 사용으로 판정
	// Anim Play 성공 = 스킬 사용으로 판단 및 쿨타임 적용
	if ( InSkillAnimInfo.SkillAnimServerKey == NormalSkillAnimInfo.SkillAnimServerKey )
	{
		if ( OnBeginSkill.IsBound() )
			OnBeginSkill.Broadcast();
		
		// server & autonomous only
		if( GetOwnerRole() == ROLE_SimulatedProxy )
			return;
		
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
	}
}

/**
 *  Anim 종료 시 호출.
 * @param InSkillAnimInfo : End된 Skill Anim 정보
 */
void UR4Skill_PlayerNormal::OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	Super::OnEndSkillAnim( InSkillAnimInfo );

	// Normal Skill의 경우 Anim End 시점을 Skill End로 판정
	if ( InSkillAnimInfo.SkillAnimServerKey == NormalSkillAnimInfo.SkillAnimServerKey )
	{
		if ( OnEndSkill.IsBound() )
			OnEndSkill.Broadcast();
	}
}
