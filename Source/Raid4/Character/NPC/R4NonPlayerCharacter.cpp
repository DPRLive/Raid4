﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerCharacter.h"

#include "../../Controller/R4AIController.h"
#include "../../Skill/NonPlayer/R4NonPlayerSkillComponent.h"
#include "../../Damage/R4DamageStruct.h"

#include <Components/CapsuleComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerCharacter)

AR4NonPlayerCharacter::AR4NonPlayerCharacter( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer.SetDefaultSubobjectClass<UR4NonPlayerSkillComponent>( FName( TEXT( "SkillComp" ) ) ) )
{
	PrimaryActorTick.bCanEverTick = false;

	CharacterDTKey = DTConst::G_InvalidPK;
	MaxPatrolRadius = 800.f;
	AIRotationSpeed = 300.f;
	
	// AI Controller 지정
	AIControllerClass = AR4AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// Set Profile Enemy
	if ( GetCapsuleComponent() )
		GetCapsuleComponent()->SetCollisionProfileName( Collision::G_ProfileEnemy );
}

void AR4NonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Begin Play 이전 Push된 Character DT Key가 존재하지 않으면,
	// 레벨 배치 시 적용한 ID를 사용
	if( CachedCharacterDTKey == DTConst::G_InvalidPK )
		PushDTData( CharacterDTKey );
}

void AR4NonPlayerCharacter::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	CachedDamagedControllers.Empty( );
	Super::EndPlay( EndPlayReason );
}

/**
 *  스킬 사용 요청 처리
 *  @return : Skill 사용 성공 시, Skill의 종료를 알리는 Delegate
 */
FSimpleMulticastDelegate* AR4NonPlayerCharacter::ActivateAISkill( uint8 InSkillIndex )
{
	if ( UR4NonPlayerSkillComponent* skillComp = Cast<UR4NonPlayerSkillComponent>( SkillComp ) )
		return skillComp->ActivateAISkill( InSkillIndex );
	
	return nullptr;
}

/**
 *  사용 가능한 스킬 중, ActivateSkillMinDist가 큰 Skill Index를 반환
 */
int32 AR4NonPlayerCharacter::GetAvailableMaxDistSkillIndex( float& OutDist ) const
{
	if ( UR4NonPlayerSkillComponent* skillComp = Cast<UR4NonPlayerSkillComponent>( SkillComp ) )
		return skillComp->GetAvailableMaxDistSkillIndex( OutDist );

	return INDEX_NONE;
}

/**
 *	AI Character가 Damage 수신 시 로직
 */
void AR4NonPlayerCharacter::ReceiveDamage( AActor* InInstigator, const FR4DamageReceiveInfo& InDamageInfo )
{
	Super::ReceiveDamage( InInstigator, InDamageInfo );

	// AI가 받은 Damage와 Damage를 가한 controller를 기억
	const AController* instigatorController = Cast<AController>( InInstigator );
	if( !IsValid( instigatorController ) )
	{
		// InInstigator가 Controller가 아닌 경우. APawn이라면 controller를 get
		if ( const APawn* instigator = Cast<APawn>( InInstigator ) )
			instigatorController = instigator->GetController();
	}

	if( !IsValid( instigatorController ) )
		return;

	float& totalDamage = CachedDamagedControllers.FindOrAdd( instigatorController );
	totalDamage += InDamageInfo.IncomingDamage;
}
