﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerState.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerState)

AR4PlayerState::AR4PlayerState()
{
	SelectedCharacterId = DTConst::G_InvalidPK;
}

void AR4PlayerState::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	// 선택한 Character Number Replicate.
	DOREPLIFETIME( AR4PlayerState, SelectedCharacterId );
}

void AR4PlayerState::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnSetCharacterIdDelegate.Clear();
	OnSetPlayerNameDelegate.Clear();
	Super::EndPlay( EndPlayReason );
}

void AR4PlayerState::CopyProperties( APlayerState* InPlayerState )
{
	Super::CopyProperties( InPlayerState );

	if ( IR4PlayerStateInterface* newPlayerState = Cast<IR4PlayerStateInterface>( InPlayerState ) )
		newPlayerState->SetCharacterId( SelectedCharacterId );
}

void AR4PlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	if( OnSetPlayerNameDelegate.IsBound() )
		OnSetPlayerNameDelegate.Broadcast( GetPlayerName() );
}

void AR4PlayerState::_OnRep_SelectedCharacterId() const
{
	if( OnSetCharacterIdDelegate.IsBound() )
		OnSetCharacterIdDelegate.Broadcast( SelectedCharacterId );
}
