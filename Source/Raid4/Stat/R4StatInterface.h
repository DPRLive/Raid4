// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4StatInterface.generated.h"

class UR4StatBaseComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4StatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * StatComp를 들고있는 객체의 StatComp에 접근하기 위한 interface.
 */
class RAID4_API IR4StatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// StatComp를 리턴
	virtual UR4StatBaseComponent* GetStatComponent() = 0;
};
