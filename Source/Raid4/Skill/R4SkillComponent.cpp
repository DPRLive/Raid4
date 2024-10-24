// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillComponent.h"
#include "R4SkillBase.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillComponent)

UR4SkillComponent::UR4SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UR4SkillComponent, SkillInstances, COND_OwnerOnly);
}

void UR4SkillComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	ClearSkills();
	OnSkillCooldownDelegate.Clear();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *  스킬을 추가 (서버)
 */
void UR4SkillComponent::Server_AddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill )
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.") ) )
		return;

	// 기존 Skill이 존재하면, 제거
	if ( SkillInstances.IsValidIndex( InSkillIndex ) )
	{
		if ( IsValid( SkillInstances[InSkillIndex] ) )
		{
			PreRemoveSkill( InSkillIndex, SkillInstances[InSkillIndex] );
			SkillInstances[InSkillIndex]->DestroyComponent();
		}

		SkillInstances[InSkillIndex] = nullptr;
	}
	
	// 인덱스가 부족하면, resize
	if ( InSkillIndex >= SkillInstances.Num() )
	{
		SkillInstances.SetNum( InSkillIndex + 1 );
	}

	SkillInstances[InSkillIndex] = InSkill;
	PostAddSkill( InSkillIndex, SkillInstances[InSkillIndex] );
}

/**
 *  관리중이던 Skill들을 모두 해제.
 */
void UR4SkillComponent::ClearSkills()
{
	// Skill Comp 제거는 Server에서
	if( GetOwnerRole() == ROLE_Authority )
	{
		for( auto& skill : SkillInstances )
			skill->DestroyComponent();
		SkillInstances.Empty();
	}
}

/**
 *  Cooldown getter
 *  @return : 해당 Skill index에 해당하는 남은 쿨타임, 유효하지 않을 시 -1 반환.
 */
float UR4SkillComponent::GetSkillCooldownRemaining( int32 InSkillIndex ) const
{
	if ( SkillInstances.IsValidIndex(InSkillIndex) && IsValid( SkillInstances[InSkillIndex] ) )
		return SkillInstances[InSkillIndex]->GetSkillCooldownRemaining();

	return -1.f;
}

/**
 *  Skill에 Cooldown이 적용 시 broadcast.
 */
void UR4SkillComponent::PostAddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill )
{
	if ( !IsValid( InSkill ) )
		return;
		
	InSkill->OnSetSkillCooldown.AddWeakLambda( this,
		[this, InSkillIndex]()
		{
			if ( OnSkillCooldownDelegate.IsBound() )
				OnSkillCooldownDelegate.Broadcast( InSkillIndex );
		} );
}

void UR4SkillComponent::_OnRep_SkillInstances( const TArray<UR4SkillBase*>& InPrev )
{
	// 제거 되어야 하는 스킬들에 대해 PreRemove 호출
	for ( int32 i = 0; i < InPrev.Num(); i++ )
	{
		// 내용이 안 바뀐 경우
		if ( SkillInstances.IsValidIndex( i ) && ( InPrev[i] == SkillInstances[i] ) )
			continue;

		PreRemoveSkill( i, InPrev[i] );
	}

	// 새로 추가 된 스킬들에 대해 PostAddSkill 호출
	for ( int32 i = 0; i < SkillInstances.Num(); i++ )
	{
		// 내용이 안 바뀐 경우
		if ( InPrev.IsValidIndex( i ) && ( InPrev[i] == SkillInstances[i] ) )
			continue;

		PostAddSkill( i, SkillInstances[i] );
	}
}
