// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4PlayerStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Player State를 위한 인터페이스
 */
class RAID4_API IR4PlayerStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Character ID ( DT Key ) 를 반환
	virtual int32 GetCharacterId() const = 0;

	// Character ID ( DT Key ) 를 설정
	virtual void SetCharacterId( int32 InCharacterId ) = 0;
};
