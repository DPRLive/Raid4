// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4NotifyByIdInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UR4NotifyByIdInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 특정 id 기반으로 delegate를 등록 / 해제 할 수 있는 Notify를 위한 interface.
 */
class RAID4_API IR4NotifyByIdInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Notify를 받을 수 있는 Key로 구분된 Delegate 반환
	 * @param InKey : 요청을 구분할 수 있는 Key (중복x)
	 */
	virtual FSimpleDelegate& OnNotify( int32 InKey ) = 0;

	/**
	 * 특정 Key에 대해 연결된 Delegate를 Unbind
	 * @param InKey : 요청을 구분할 수 있는 Key (중복x)
	 */
	virtual void UnbindNotify( int32 InKey ) = 0;

	/**
 	 * 해당 Notify의 Type을 반환
 	 */
	virtual ER4AnimNotifyType GetNotifyType() const = 0;
};
