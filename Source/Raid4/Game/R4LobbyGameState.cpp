// Fill out your copyright notice in the Description page of Project Settings.


#include "R4LobbyGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4LobbyGameState)

AR4LobbyGameState::AR4LobbyGameState()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AR4LobbyGameState::AddPlayerState( APlayerState* PlayerState )
{
	Super::AddPlayerState( PlayerState );

	if( OnChangePlayerArrayDelegate.IsBound() )
		OnChangePlayerArrayDelegate.Broadcast();
}

void AR4LobbyGameState::RemovePlayerState( APlayerState* PlayerState )
{
	Super::RemovePlayerState( PlayerState );

	if( OnChangePlayerArrayDelegate.IsBound() )
		OnChangePlayerArrayDelegate.Broadcast();
}


