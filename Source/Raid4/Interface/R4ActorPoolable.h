// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4ActorPoolable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4ActorPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * ActorPool에 의해 Pooling 될 수 있는 Actor를 정의 시 사용
 */
class RAID4_API IR4ActorPoolable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Actor를 Pool에서 Get하기 전 처리할 로직을 정의
	virtual void PreGetPoolActor() = 0;

	// Actor를 Pool에 Return하기 전 처리할 로직을 정의
	virtual void PreReturnPoolActor() = 0;
};
