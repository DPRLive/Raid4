// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4StatusBarInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4StatusBarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * status bar를 구현시 Interface.
 */
class RAID4_API IR4StatusBarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 최대 체력을 설정
	virtual void SetTotalHp( float InTotalHp ) = 0;
	
	// 현재 체력을 설정
	virtual void SetCurrentHp( float InCurrentHp ) = 0;

	// 현재 쉴드량을 설정
	virtual void SetCurrentShieldAmount( float InCurrentShieldAmount ) = 0;

	// status bar를 초기화
	virtual void Clear() = 0;
};
