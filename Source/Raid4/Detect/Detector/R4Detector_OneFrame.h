// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DetectorInterface.h"
#include "../../Core/ObjectPool/PoolableActor.h"

#include <GameFramework/Actor.h>

#include "R4Detector_OneFrame.generated.h"

/**
 *  Overlap 체크시 사용할 모양
 */
UENUM()
enum class ER4DetectShapeType : uint8
{
	Box			UMETA( DisplayName = "박스" ),
	Sphere		UMETA( DisplayName = "구" ),
	Capsule		UMETA( DisplayName = "캡슐" ),
	Sector		UMETA( DisplayName = "부채꼴" )
};

/**
 *  Detect할 모양과 관련 정보 설정
 */
USTRUCT()
struct FR4DetectShapeInfo
{
	GENERATED_BODY()

	FR4DetectShapeInfo()
	: Shape( ER4DetectShapeType::Box )
	, BoxHalfExtent( FVector::ZeroVector )
	, Radius( 0.f )
	, HalfHeight( 0.f )
	, Angle( 0.f )
	{}
	
	// Overlap할 형태 결정
	UPROPERTY( EditAnywhere, Category="Collision", meta =(AllowPrivateAccess = true) )
	ER4DetectShapeType Shape;
	
	// 반지름
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =(  EditCondition = "Shape == ER4DetectShapeType::Box", EditConditionHides, AllowPrivateAccess = true) )
	FVector BoxHalfExtent;
	
	// 반지름
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape != ER4DetectShapeType::Box", EditConditionHides, AllowPrivateAccess = true) )
	float Radius;

	// HalfHeight
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape == ER4DetectShapeType::Capsule || Shape == ER4DetectShapeType::Sector", EditConditionHides, AllowPrivateAccess = true) )
	float HalfHeight;

	// angle
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape == ER4DetectShapeType::Sector", EditConditionHides, AllowPrivateAccess = true) )
	float Angle;
};

/**
 * 1프레임만 Detect를 수행하는 클래스.
 */
UCLASS( Abstract, HideCategories = ( Tick, Replication, Rendering, Actor, Input, HLOD, Physics, LevelInstance, WorldPartition, DataLayers ) )
class RAID4_API AR4Detector_OneFrame : public APoolableActor,
										public IR4DetectorInterface
{
	GENERATED_BODY()

public:
	AR4Detector_OneFrame();

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
	virtual void ExecuteDetect( const AActor* InInstigator, const FR4DetectDesc& InDetectDesc ) override;
	// ~ End IR4DetectorInterface
private:
	// Detect에 사용할 모양 정보
	UPROPERTY( EditAnywhere, Category="Collision", meta =(AllowPrivateAccess = true) )
	FR4DetectShapeInfo ShapeInfo;

	// Detect 시작 시 Broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 Broadcast. OneFrame Check의 경우 OnBegin과 동시에 End.
	FOnDetectDelegate OnEndDetectDelegate;

	// TODO : Debug 분리 ?
	// Debug 할 것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	uint8 bDrawDebug:1;

	// Debug를 얼마나 그릴것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	float DebugTime;
	
	// Debug Color 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	FColor DebugColor;
};