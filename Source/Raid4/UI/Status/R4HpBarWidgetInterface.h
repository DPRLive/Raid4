// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4HpBarWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4HpBarWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * HP bar를 표시할 수 있는 객체를 위한 Interface
 */
class RAID4_API IR4HpBarWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// SetUp을 진행
	virtual void SetupHpBarWidget( UUserWidget* InWidget ) = 0;
};
