// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameMode.h>
#include "R4GameMode.generated.h"

class AR4PlayerController;

/**
 * Game Mode.
 * Player와 NPC의 싸우는 것이 게임의 진행이고, Player나 NPC가 모두 죽으면 게임 종료.
 */
UCLASS()
class RAID4_API AR4GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AR4GameMode();

protected:
	// PostLogin / Seamless travel 시 호출.
	virtual void HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer ) override;

	// 게임 시작
	virtual void HandleMatchHasStarted() override;

	// 게임 종료
	virtual void HandleMatchHasEnded() override;

private:
	// Player 사망
	UFUNCTION()
	void _PlayerDead( AActor* InDeadActor );

	// NPC 사망
	UFUNCTION()
	void _NPCDead( AActor* InDeadActor );
	
	// Match 상태를 확인
	void _CheckMatchState();
	
private:
	// 몇명의 플레이어가 Travel 되면 게임을 시작하는지 설정 
	UPROPERTY( EditDefaultsOnly )
	uint8 NumPlayersToStartGame;

	// Player 사망 시 ASpectatorPawn로 변경하는 Delay
	UPROPERTY( EditDefaultsOnly )
	float ToSpectatorDelay;

	// 현재 게임에 참여한 { Player, 사용 중인 Pawn }
	UPROPERTY( Transient, VisibleInstanceOnly )
	TMap<TWeakObjectPtr<APlayerController>, TWeakObjectPtr<APawn>> CachedPlayers;
	
	// 살아있는 Player
	int32 CachedNumAlivePlayers;
	
	// 살아있는 NPC
	int32 CachedNumAliveNPCs;
};
