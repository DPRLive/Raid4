// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerState.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerState)

AR4PlayerState::AR4PlayerState()
{
	// 우히힝 테스틍
	SelectedCharacterId = 1;
}

void AR4PlayerState::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	// 선택한 Character Number Replicate.
	DOREPLIFETIME( AR4PlayerState, SelectedCharacterId );
}

void AR4PlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AR4PlayerState::CopyProperties( APlayerState* InPlayerState )
{
	Super::CopyProperties( InPlayerState );

	if ( IR4PlayerStateInterface* newPlayerState = Cast<IR4PlayerStateInterface>( InPlayerState ) )
		newPlayerState->SetCharacterId( SelectedCharacterId );
}


