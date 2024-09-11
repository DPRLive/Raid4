// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4TagStatQueryInterface.generated.h"

struct FR4StatInfo;
struct FR4CurrentStatInfo;

// This class does not need to be modified.
UINTERFACE()
class UR4TagStatQueryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 특정 객체가 Tag로 Stat을 쿼리할 수 있도록 하는 기능 제공 시 사용
 */
class RAID4_API IR4TagStatQueryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Tag에 맞는 FR4StatInfo 형태의 Stat을 반환.
	virtual FR4StatInfo* GetStatByTag( const FGameplayTag& InTag ) const = 0;

	// Tag에 맞는 FR4CurrentStatInfo 형태의 Stat 반환.
	virtual FR4CurrentStatInfo* GetCurrentStatByTag( const FGameplayTag& InTag ) const = 0;
};
