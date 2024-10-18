// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameState.h>
#include "R4GameState.generated.h"

/**
 * R4GameState, 현재 게임 진행 정보를 저장
 */
UCLASS()
class RAID4_API AR4GameState : public AGameState
{
	GENERATED_BODY()

public:
	AR4GameState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
public:
	// Getter
	FORCEINLINE int32 GetNumAlivePlayers() const { return CachedNumAlivePlayers; }	
	FORCEINLINE int32 GetNumAliveNPCs() const { return CachedNumAliveNPCs; }

	// Game State 변동 알림
	FSimpleMulticastDelegate OnUpdateGameState;
	
protected:
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

	// Game State Update
	UFUNCTION()
	void _UpdateGameState() const;

	// Game End 후 처리
	void _AfterEndGame();
private:
	// 게임 종료 시 UI (Player Win)
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UUserWidget> PlayerWinWidget;

	// 게임 종료 시 UI (Player Defeat)
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UUserWidget> PlayerDeFeatWidget;

	// 게임 종료 처리 Delay.
	UPROPERTY( EditDefaultsOnly )
	float GameEndDelay;

	// 게임 종료 처리 Delay Timer
	FTimerHandle CachedGameEndDelayTimer;
	
	// 살아있는 Player
	UPROPERTY( ReplicatedUsing = _UpdateGameState )
	int32 CachedNumAlivePlayers;
	
	// 살아있는 NPC
	UPROPERTY( ReplicatedUsing = _UpdateGameState )
	int32 CachedNumAliveNPCs;
};