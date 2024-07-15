// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4MouseMoveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4MouseMoveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 마우스 이동을 지원하는 객체를 위한 인터페이스
 * (PlayerInputComp 사용 시 사용)
 */
class RAID4_API IR4MouseMoveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void StopMove() = 0;
	virtual void AddMovement(const FVector& InWorldDir) = 0;
	virtual void MoveToLocation(const FVector& InLoc) = 0;
};