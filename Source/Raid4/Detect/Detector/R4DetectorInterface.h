// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4DetectorInterface.generated.h"

struct FR4DetectDesc;
struct FR4DetectResult;

// Detect 결과를 돌려주는 Delegate
DECLARE_MULTICAST_DELEGATE_OneParam( FOnDetectDelegate, const FR4DetectResult& /* InDetectResult */);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4DetectorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 레벨에서 무언가 탐지를 할 수 있는 오브젝트 정의 시 사용
 */
class RAID4_API IR4DetectorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 탐지 시작 시 broadcast하는 delegate를 반환
	 */
	virtual FOnDetectDelegate& OnBeginDetect() = 0;

	/**
	 * 탐지 종료 시 broadcast하는 delegate를 반환
	 */
	virtual FOnDetectDelegate& OnEndDetect() = 0;

	/**
 	* Detect 준비
 	* @param InOrigin : 탐지의 기준이 되는 Transform
 	* @param InDetectDesc : 탐지 관련 Param 
 	*/
	virtual void SetupDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc ) = 0;
	
	/**
	 * Detect 실행
	 */
	virtual void ExecuteDetect( ) = 0;
};