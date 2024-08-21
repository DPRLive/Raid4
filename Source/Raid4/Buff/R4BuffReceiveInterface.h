// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4BuffReceiveInterface.generated.h"

struct FR4BuffSettingDesc;
class UR4BuffBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4BuffReceiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Buff를 받을 수 있는 객체를 위한 인터페이스.
 */
class RAID4_API IR4BuffReceiveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	*  Buff를 받는 함수
	*  @param InInstigator : 버프 시전한 액터
	*  @param InBuffClass : 버프 클래스
	*  @param InBuffSettingDesc : 버프 사용 관련 설정
	*/
	virtual void ReceiveBuff(AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc) = 0;
};
