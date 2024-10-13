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
	// SetUp을 진행
	virtual void SetupStatusBarWidget(UUserWidget* InWidget) = 0;
};
