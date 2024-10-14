// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/PlayerController.h>
#include "R4PlayerController.generated.h"

/**
 * Player가 사용할 Player Controller
 */
UCLASS()
class RAID4_API AR4PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AR4PlayerController();

	virtual void BeginPlay() override;
	
	// Character ID 선택 요청 전송.
	UFUNCTION( Server, Unreliable )
	void RequestCharacterPick( int32 InCharacterId );
};
