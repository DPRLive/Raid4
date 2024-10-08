// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerCharacter.h"

#include "../../Controller/R4AIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerCharacter)

AR4NonPlayerCharacter::AR4NonPlayerCharacter( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer )
{
	PrimaryActorTick.bCanEverTick = false;

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

	float& totalDamage = DamagedControllerList.FindOrAdd( instigatorController );
	totalDamage += InDamage;
}