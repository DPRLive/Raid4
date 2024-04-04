// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4DTBasedInitable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4DTBasedInitable : public UInterface
{
	GENERATED_BODY()
};

/**
 * DT 데이터에 의해 초기화 될 수 있는 클래스를 위한 인터페이스
 */
class RAID4_API IR4DTBasedInitable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 해당 클래스를 초기화 하기 위한 알맞는 DT의 Pk를 통해 데이터 초기화
	virtual void InitializeByDTPriKey(FPriKey InPk) = 0;
};
