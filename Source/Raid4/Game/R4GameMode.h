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

	// Init Game, Option에 설정된 Player Num 체크
	virtual void InitGame(const FString& InMapName, const FString& InOptions, FString& InErrorMessage) override;

protected:
	// PostLogin / Seamless travel 시 호출.
	virtual void HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer ) override;
	
	// Spawn Player Pawn
	virtual void RestartPlayer(AController* InNewPlayer) override;
	
	// 게임 시작
	virtual void HandleMatchHasStarted() override;

	// Game 시작 타이밍을 수동으로 설정
	virtual bool ReadyToStartMatch_Implementation() override { return false; }
private:
	// Player 사망
	UFUNCTION()
	void _PlayerDead( AActor* InDeadActor );
	
	// Match 상태를 확인
	void _CheckMatchState();
	
private:
	// Player 사망 시 ASpectatorPawn로 변경하는 Delay
	UPROPERTY( EditDefaultsOnly )
	float ToSpectatorDelay;

	// 게임에 참여하는 총 Player 수
	UPROPERTY( Transient, VisibleInstanceOnly )
	int32 CachedInGamePlayerNums;
	
	// 현재 게임에 참여한 { Player, 사용 중인 Pawn }
	UPROPERTY( Transient, VisibleInstanceOnly )
	TMap<TWeakObjectPtr<AController>, TWeakObjectPtr<APawn>> CachedPlayers;
};
