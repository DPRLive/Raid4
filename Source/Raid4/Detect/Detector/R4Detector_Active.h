// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DetectorInterface.h"
#include "../../Core/ObjectPool/PoolableActor.h"

#include "R4Detector_Active.generated.h"

/**
 * Capsule, Box, Sphere 등 UShapeComponent 기반으로 하는 Comp를 통해
 * Detect를 실행.
 * 
 */
UCLASS( Abstract )
class RAID4_API AR4Detector_Active : public APoolableActor,
										public IR4DetectorInterface
{
	GENERATED_BODY()

public:
	AR4Detector_Active();

protected:
	virtual void BeginPlay() override;

public:
	// ~ Begin IPoolableInterface
	virtual void PostInitPoolObject() override {}
	virtual void PreReturnPoolObject() override;
	// ~ End IPoolableInterface
	
	// ~ Begin IR4DetectorInterface
	FORCEINLINE virtual FOnDetectDelegate& OnBeginDetect() override { return OnBeginDetectDelegate; }
	FORCEINLINE virtual FOnDetectDelegate& OnEndDetect() override { return OnEndDetectDelegate; }
	virtual void ExecuteDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc ) override;
	// ~ End IR4DetectorInterface

private:
	// Detect 시작 시 Broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 Broadcast. OneFrame Check의 경우 OnBegin과 동시에 End.
	FOnDetectDelegate OnEndDetectDelegate;
};
