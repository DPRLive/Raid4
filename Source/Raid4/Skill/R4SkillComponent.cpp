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
 *	컴포넌트 초기화
 */
void UR4SkillComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UR4SkillComponent, SkillInstances, COND_OwnerOnly);
}

/**
 *  begin play
 */
void UR4SkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
