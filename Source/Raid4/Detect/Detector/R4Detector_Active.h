// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DetectorInterface.h"
#include "../../Core/ObjectPool/PoolableActor.h"

#include "R4Detector_Active.generated.h"

class UShapeComponent;

/**
 * Capsule, Box, Sphere 등 UShapeComponent 기반으로 하는 Comp를 통해 Detect를 실행.
 * 해당 Actor에 멤버로 등록 된 UShapeComponent 들은 모두 파싱되어 Detect에 이용되며,
 * 중첩 적용. ( 2개의 Shape가 해당 Actor의 멤버로 등록되고, 임의의 객체가 두개에 동시에 겹치면 두번 탐지됨 )
 * ShapeComp들은 disable collision 설정되어 있으며 ExecuteDetect()시 enable 되어 사용됨.
 */
UCLASS( Abstract, HideCategories = ( Tick, Rendering, Actor, Input, HLOD, Physics, LevelInstance, WorldPartition, DataLayers ))
class RAID4_API AR4Detector_Active : public APoolableActor,
										public IR4DetectorInterface
{
	GENERATED_BODY()

public:
	AR4Detector_Active();

protected:
	virtual void BeginPlay() override;

public:
	// ~ Begin APoolableActor
	virtual void PostInitPoolObject() override {}
	virtual void PreReturnPoolObject() override;
	// ~ End APoolableActor
	
	// ~ Begin IR4DetectorInterface
	FORCEINLINE virtual FOnDetectDelegate& OnBeginDetect() override { return OnBeginDetectDelegate; }
	FORCEINLINE virtual FOnDetectDelegate& OnEndDetect() override { return OnEndDetectDelegate; }
	virtual void SetupDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc ) override;
	virtual void TearDownDetect() override;
	virtual void ExecuteDetect() override;
	// ~ End IR4DetectorInterface

protected:
	// BP에서 Setup시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName = "SetupDetect") )
	void BP_SetupDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc );

	// BP에서 TearDown시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName = "TearDownDetect") )
	void BP_TearDownDetect();
	
private:
	// 멤버로 등록된 Shape Comp Begin Overlap 시 호출
	UFUNCTION()
	void _OnBeginShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	// 멤버로 등록된 Shape Comp End Overlap 시 호출
	UFUNCTION()
	void _OnEndShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

private:
	// Detect 시작 시 Broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 Broadcast. OneFrame Check의 경우 OnBegin과 동시에 End.
	FOnDetectDelegate OnEndDetectDelegate;
	
	// Detect 생명주기를 위한 Timer
	FTimerHandle LifeTimerHandle;
};
