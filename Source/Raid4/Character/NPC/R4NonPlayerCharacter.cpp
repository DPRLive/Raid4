// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerCharacter.h"

#include "../../Controller/R4AIController.h"
#include "../../Skill/NonPlayer/R4NonPlayerSkillComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerCharacter)

AR4NonPlayerCharacter::AR4NonPlayerCharacter( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer.SetDefaultSubobjectClass<UR4NonPlayerSkillComponent>( FName( TEXT( "SkillComp" ) ) ) )
{
	PrimaryActorTick.bCanEverTick = false;

	MaxPatrolRadius = 800.f;
	
	// AI Controller 지정
	AIControllerClass = AR4AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AR4NonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnCharacterDamagedDelegate.AddDynamic( this, &AR4NonPlayerCharacter::OnAICharacterDamaged );
}

/**
 *  스킬 사용 요청 처리
 */
void AR4NonPlayerCharacter::ActivateAISkill( uint8 InSkillIndex )
{
	if ( UR4NonPlayerSkillComponent* skillComp = Cast<UR4NonPlayerSkillComponent>( SkillComp ) )
		skillComp->ActivateAISkill( InSkillIndex );
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
void AR4NonPlayerCharacter::OnAICharacterDamaged( const AActor* InInstigator, float InDamage )
{
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
	totalDamage += InDamage;
}