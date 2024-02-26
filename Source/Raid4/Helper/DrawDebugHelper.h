#pragma once

#if UE_ENABLE_DEBUG_DRAWING

/**
* DrawDebugHelpers에서 지원하지 않는 기능을 위한 DrawDebug helper
*/
namespace DrawDebugHelper
{
	/**
	 * 기존 DrawDebugCircleArc에서 Y축을 추가한 함수 
	 */
	void DrawDebugCircleArcXY(const UWorld* InWorld, const FVector& InCenter, float InRadius, const FVector& InAxisX, const FVector& InAxisY, float InAngleWidth, int32 InSegments, const FColor& InColor, bool InPersistentLines = false, float InLifeTime = -1.f, uint8 InDepthPriority = 0, float InThickness = 0.f);

	/**
	 * 부채꼴 기둥을 그려주는 함수
	 */
	void DrawDebugSector(const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InRadian, int32 InSegments, const FColor& InColor, bool InPersistentLines = false, float InLifeTime = -1.f, uint8 InDepthPriority = 0, float InThickness = 0.f);
}

#endif