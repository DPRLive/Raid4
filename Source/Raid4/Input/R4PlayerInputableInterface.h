// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerInputableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4PlayerInputableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 객체가 플레이어 입력 처리를 해야할 때를 위한 인터페이스 
 */
class RAID4_API IR4PlayerInputableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 입력 시작
	virtual void OnInputStarted() = 0;

	// 입력 중
	virtual void OnInputTriggered() = 0;

	// 입력 종료
	virtual void OnInputCompleted() = 0;
};
