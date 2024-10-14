// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerController.h"

#include "TimerManager.h"
#include "../Game/R4LobbyGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerController)

AR4PlayerController::AR4PlayerController()
{
}

void AR4PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if( !IsLocalController() )
		return;

	static int32 testCharacterID = 1;
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer( handle, [this]()
	{
		RequestCharacterPick( testCharacterID++ );
	}, 3.f, false);
}

/**
 *	캐릭터 선택 요청
 *	@param InCharacterId : 선택할 Character의 DT PK
 */
void AR4PlayerController::RequestCharacterPick_Implementation( int32 InCharacterId )
{
	// Lobby Game Mode에 요청
	AR4LobbyGameMode* gameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AR4LobbyGameMode>() : nullptr;
	if( IsValid( gameMode ) )
		gameMode->RequestCharacterPick( this, InCharacterId );
}