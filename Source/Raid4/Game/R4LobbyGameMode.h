// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4CharacterPickGameModeInterface.h"

#include <GameFramework/GameModeBase.h>

#include "R4LobbyGameMode.generated.h"

/**
 * Lobby & Champion Select 화면에서 사용할 GameMode.
 */
UCLASS()
class RAID4_API AR4LobbyGameMode : public AGameModeBase, public IR4CharacterPickGameModeInterface
{
	GENERATED_BODY()

public:
	AR4LobbyGameMode();
	
	// ~ Begin IR4CharacterPickGameModeInterface
	virtual void RequestCharacterPick( APlayerController* InReqController, int32 InCharacterId ) override;
	// ~ End IR4CharacterPickGameModeInterface

	bool CanTravelMainLevel() const;
protected:
	// PostLogin / Seamless travel 시 호출.
	virtual void HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer ) override;

private:
	// 현재 게임에 참여한 Player의 Controllers.
	UPROPERTY( Transient, VisibleInstanceOnly )
	TSet<TWeakObjectPtr<APlayerController>> CachedPlayerControllers;
};
