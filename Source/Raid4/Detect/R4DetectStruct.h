#pragma once

#include <Components/PrimitiveComponent.h>
#include "R4DetectStruct.generated.h"

/**
 * Detect 요청 시 사용할 정보
 */
USTRUCT()
struct FR4DetectDesc
{
	GENERATED_BODY()

	FR4DetectDesc()
	: RelativeLoc( FVector::ZeroVector )
	, RelativeRot( FRotator::ZeroRotator )
	{}

	// overlap을 체크할 기준점으로부터의 상대 위치.
	UPROPERTY( EditAnywhere )
	FVector RelativeLoc;
	
	// Overlap시 기준으로부터 상대적인 Rotation. X : Roll, Y : Pitch, Z : Yaw
	UPROPERTY( EditAnywhere )
	FRotator RelativeRot;
	
	// Overlap 체크 시 사용할 Collision Response를 위한 BodyInstance
	UPROPERTY( EditAnywhere, meta=(ShowOnlyInnerProperties, SkipUCSModifiedProperties, AllowPrivateAccess = true) )
	FBodyInstance BodyInstance;
};

/**
 * Detect한 결과 정보.
 */
USTRUCT()
struct FR4DetectResult
{
	GENERATED_BODY()

	FR4DetectResult()
	: Location(FVector_NetQuantize::ZeroVector)
	, DetectedComponent(nullptr)
	, DetectedActor(nullptr)
	{}

	FR4DetectResult(const FVector& InLoc, UPrimitiveComponent* InDetectedComp, AActor* InDetectedActor)
	: Location(InLoc)
	, DetectedComponent(InDetectedComp)
	, DetectedActor(InDetectedActor)
	{}
	
	// 발견한 위치
	UPROPERTY( Transient )
	FVector_NetQuantize Location;

	// 발견된 Component
	UPROPERTY( Transient )
	TWeakObjectPtr<UPrimitiveComponent> DetectedComponent;

	// 발견된 Actor
	UPROPERTY( Transient )
	TWeakObjectPtr<AActor> DetectedActor;
};
