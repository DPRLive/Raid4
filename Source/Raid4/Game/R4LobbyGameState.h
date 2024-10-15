// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameStateBase.h>
#include "R4LobbyGameState.generated.h"

/**
 * Lobby에서 사용하는 Game State.
 * Player Array가 변경되면 알림.
 */
UCLASS()
class RAID4_API AR4LobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AR4LobbyGameState();

	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
public:
	// Player State 추가
	virtual void AddPlayerState( APlayerState* PlayerState ) override;

	// Player State 제거
	virtual void RemovePlayerState( APlayerState* PlayerState ) override;

public:
	// Player State Array의 변동을 알림.
	FSimpleMulticastDelegate OnChangePlayerArrayDelegate;
};
