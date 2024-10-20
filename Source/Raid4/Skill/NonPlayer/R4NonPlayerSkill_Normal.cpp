﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerSkill_Normal.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerSkill_Normal)

UR4NonPlayerSkill_Normal::UR4NonPlayerSkill_Normal()
{
	ActivateSkillMinDist = 0.f;
}

void UR4NonPlayerSkill_Normal::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	// Anim Key를 Replicate 받을 수 있도록 설정
	DOREPLIFETIME_CONDITION( UR4NonPlayerSkill_Normal, SkillAnimInfo, COND_InitialOnly );
}

/**
 * AI Skill을 실행.
 */
void UR4NonPlayerSkill_Normal::ActivateAISkill()
{
	if ( CanActivateSkill() )
		PlaySkillAnim_WithServerRPC( SkillAnimInfo, NAME_None );
}

/**
 * Anim을 Play시작 시 호출
 * @param InSkillAnimInfo : Play된 Skill Anim 정보
 * @param InStartSectionName : 시작 된 Section Name.
 * @param InStartServerTime : Skill Anim이 시작된 Server Time
 */
void UR4NonPlayerSkill_Normal::OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	Super::OnBeginSkillAnim( InSkillAnimInfo, InStartSectionName, InStartServerTime );

	// Anim Play 시점을 Skill 사용으로 판정
	// Anim Play 성공 = 스킬 사용으로 판단 및 쿨타임 적용
	if ( InSkillAnimInfo.SkillAnimServerKey == SkillAnimInfo.SkillAnimServerKey )
	{
		if ( OnBeginSkill.IsBound() )
			OnBeginSkill.Broadcast();
		
		// server only
		if ( GetOwnerRole() != ROLE_Authority )
			return;
		
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
	}
}

/**
 *  Anim 종료 시 호출.
 * @param InSkillAnimInfo : End된 Skill Anim 정보
 */
void UR4NonPlayerSkill_Normal::OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	Super::OnEndSkillAnim( InSkillAnimInfo );

	// Normal Skill의 경우 Anim End 시점을 Skill End로 판정
	if ( InSkillAnimInfo.SkillAnimServerKey == SkillAnimInfo.SkillAnimServerKey )
	{
		if ( OnEndSkill.IsBound() )
			OnEndSkill.Broadcast();
	}
}
