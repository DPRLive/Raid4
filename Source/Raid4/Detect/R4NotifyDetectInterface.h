// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4NotifyDetectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4NotifyDetectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 탐지가 필요할 때 탐지하라고 알려주는 객체를 위한 interface
 */
class RAID4_API IR4NotifyDetectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 탐지 타이밍을 broadcast로 알리는 delegate를 반환
	 * @param InReqActor : 탐지 타이밍을 알고 싶은 객체. 일반적으로 판정 시 Owner로 판정하는 경우가 많을 듯.
	 */
	virtual FSimpleDelegate& OnNotifyDetect( const AActor* InReqActor ) = 0;
	
};
