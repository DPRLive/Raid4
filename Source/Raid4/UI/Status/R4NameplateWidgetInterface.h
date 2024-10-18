// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4NameplateWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE()
class UR4NameplateWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * UR4NameplateWidget 을 사용하는 객체를 위한 인터페이스.
 */
class RAID4_API IR4NameplateWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// SetUp을 진행
	virtual void SetupNameplateWidget( UUserWidget* InWidget ) = 0;
};
