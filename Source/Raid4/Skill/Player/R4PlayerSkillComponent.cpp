// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerSkillComponent.h"
#include "../../Input/R4PlayerInputableInterface.h"
#include "../R4SkillBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerSkillComponent)

UR4PlayerSkillComponent::UR4PlayerSkillComponent()
{
}

void UR4PlayerSkillComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	SkillDelegateHandles.Empty();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *  입력 연결
 */
void UR4PlayerSkillComponent::OnInputSkillStarted( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstances.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstances[realIdx] ) )
			playerSkill->OnInputStarted();
	}
}

void UR4PlayerSkillComponent::OnInputSkillTriggered( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstances.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstances[realIdx] ) )
			playerSkill->OnInputTriggered();
	}
}

void UR4PlayerSkillComponent::OnInputSkillCompleted( EPlayerSkillIndex InSkillIndex )
{
	const uint8 realIdx = static_cast<uint8>(InSkillIndex);
	if ( SkillInstances.IsValidIndex( realIdx ) )
	{
		if ( IR4PlayerInputableInterface* playerSkill = Cast<IR4PlayerInputableInterface>( SkillInstances[realIdx] ) )
			playerSkill->OnInputCompleted();
	}
}

/**
 *  스킬을 등록 시 호출
 */
void UR4PlayerSkillComponent::PostAddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill )
{
	Super::PostAddSkill( InSkillIndex, InSkill );

	if ( !IsValid( InSkill ) )
		return;

	if ( InSkillIndex >= SkillDelegateHandles.Num() )
		SkillDelegateHandles.SetNum( InSkillIndex + 1 );

	// 시작 시 다른 스킬들을 disable.
	SkillDelegateHandles[InSkillIndex].Key = InSkill->OnBeginSkill.AddWeakLambda( this,
	[this, InSkillIndex]()
	{
		for ( int32 i = 0; i < SkillInstances.Num(); i++ )
		{
			if ( i == InSkillIndex )
				continue;

			SkillInstances[i]->SetSkillEnable( false );
		}
	} );

	// 종료 시 모두 enable로 변경.
	SkillDelegateHandles[InSkillIndex].Value = InSkill->OnEndSkill.AddWeakLambda( this,
	[this]()
	{
		for ( int32 i = 0; i < SkillInstances.Num(); i++ )
			SkillInstances[i]->SetSkillEnable( true );
	} );
}

/**
 *  스킬을 등록해제 시 호출.
 */
void UR4PlayerSkillComponent::PreRemoveSkill( uint8 InSkillIndex, UR4SkillBase* InSkill )
{
	Super::PreRemoveSkill( InSkillIndex, InSkill );

	if( !IsValid( InSkill ) || !SkillDelegateHandles.IsValidIndex( InSkillIndex ) )
		return;

	InSkill->OnBeginSkill.Remove( SkillDelegateHandles[InSkillIndex].Key );
	InSkill->OnEndSkill.Remove( SkillDelegateHandles[InSkillIndex].Value );
}
