// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4CharacterPickGameModeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4CharacterPickGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Character ID를 변경할 수 있는 GameMode를 위한 Interface
 */
class RAID4_API IR4CharacterPickGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 캐릭터 선택 요청
	virtual void RequestCharacterPick( APlayerController* InReqController, int32 InCharacterId ) = 0;
};
