// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4Overlapable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4Overlapable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 오버랩 결과를 뱉어줄 수 있는 오브젝트에 정의
 */
class RAID4_API IR4Overlapable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnBeginOverlap() {};
};
