// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/GameModeBase.h>
#include "R4LobbyGameMode.generated.h"

/**
 * Lobby & Champion Select 화면에서 사용할 GameMode.
 */
UCLASS()
class RAID4_API AR4LobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AR4LobbyGameMode();

	// Main Game으로 이동 요청
	void TravelToMainGame() const;

	// 캐릭터 선택 요청
	void RequestCharacterPick( APlayerController* InReqController, int32 InCharacterId );
protected:
	// PostLogin / Seamless travel 시 호출.
	virtual void HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer ) override;

private:
	// Transition할 Main Game Level
	UPROPERTY( EditDefaultsOnly )
	TSoftObjectPtr<UWorld> MainGameLevel;

	// 현재 게임에 참여한 Player의 Controllers.
	UPROPERTY( Transient, VisibleInstanceOnly )
	TSet<TWeakObjectPtr<APlayerController>> CachedPlayerControllers;
};
