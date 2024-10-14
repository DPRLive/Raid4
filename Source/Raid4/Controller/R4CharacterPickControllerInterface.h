// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "R4CharacterPickControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4CharacterPickControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Character ID를 선택 할 수 있는 Controller를 위한 Interface
 */
class RAID4_API IR4CharacterPickControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 캐릭터 선택 요청
	virtual void RequestCharacterPick( int32 InCharacterId ) = 0;
};
