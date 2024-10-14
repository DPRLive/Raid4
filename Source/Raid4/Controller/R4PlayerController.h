// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4CharacterPickControllerInterface.h"

#include <GameFramework/PlayerController.h>

#include "R4PlayerController.generated.h"

/**
 * Player가 사용할 Player Controller
 */
UCLASS()
class RAID4_API AR4PlayerController : public APlayerController, public IR4CharacterPickControllerInterface
{
	GENERATED_BODY()

public:
	AR4PlayerController();

	// ~ Begin IR4CharacterPickControllerInterface
	virtual void RequestCharacterPick(int32 InCharacterId) override;
	// ~ End IR4CharacterPickControllerInterface
	
private:
	// 캐릭터 선택 요청 Server RPC
	UFUNCTION( Server, Unreliable )
	void _ServerRPC_RequestCharacterPick( int32 InCharacterId );
};
