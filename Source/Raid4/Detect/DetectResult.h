#pragma once

#include <Components/PrimitiveComponent.h>
#include "DetectResult.generated.h"

/**
 * Detect한 결과 정보.
 */
USTRUCT()
struct FDetectResult
{
	GENERATED_BODY()

	FDetectResult()
	: Location(FVector_NetQuantize::ZeroVector)
	, DetectedComponent(nullptr)
	, DetectedActor(nullptr)
	{}

	FDetectResult(const FVector& InLoc, UPrimitiveComponent* InDetectedComp, AActor* InDetectedActor)
	: Location(InLoc)
	, DetectedComponent(InDetectedComp)
	, DetectedActor(InDetectedActor)
	{}
	
	// 발견한 위치
	UPROPERTY()
	FVector_NetQuantize Location;

	// 발견된 Component
	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> DetectedComponent;

	// 발견된 Actor
	UPROPERTY()
	TWeakObjectPtr<AActor> DetectedActor;
};
