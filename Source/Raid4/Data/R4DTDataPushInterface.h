// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4DTDataPushInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4DTDataPushInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * DT 데이터를 받을 수 있는 오브젝트를 정의할 때 사용
 */
class RAID4_API IR4DTDataPushInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 해당 클래스에 알맞는 DT의 Pk를 통해 데이터를 push
	virtual void PushDTData(FPriKey InPk) = 0;

	// 해당 클래스에 Push했던 DT의 Data를 Clear.
	virtual void ClearDTData() = 0;
};
