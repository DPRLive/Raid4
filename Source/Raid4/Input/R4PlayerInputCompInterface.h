// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerInputCompInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4PlayerInputCompInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * R4PlayerInputComp를 사용하기 위한 인터페이스
 */
class RAID4_API IR4PlayerInputCompInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 사용될 PlayerController를 return
	virtual APlayerController* GetPlayerController() = 0;

	// PlayerController 에서의 Input Bind 시 InputComp를 받아오기 위한 delegate getter
	DECLARE_MULTICAST_DELEGATE_OneParam( FSetupPlayerInputDelegate, UInputComponent* /* InInputComponent */ )
	virtual FSetupPlayerInputDelegate& OnSetupPlayerInput() = 0;
};