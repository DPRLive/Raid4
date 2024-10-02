#pragma once

/**
 * Overlap을 도와주는 Overlap helper
 */
namespace UtilOverlap
{
	// Box 형태의 Overlap을 체크
	bool BoxOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, const FVector& InBoxHalfExtent, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	bool BoxOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, const FVector& InBoxHalfExtent, const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& InResponseParams = FCollisionResponseParams::DefaultResponseParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	
	// Sphere 형태의 Overlap을 체크
	bool SphereOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, float InRadius, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	bool SphereOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, float InRadius, const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& InResponseParams = FCollisionResponseParams::DefaultResponseParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	
	// Capsule 형태의 Overlap을 체크
	bool CapsuleOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, float InRadius, float InHalfHeight, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	bool CapsuleOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, float InRadius, float InHalfHeight, const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& InResponseParams = FCollisionResponseParams::DefaultResponseParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);

	// Sector (부채꼴) 형태의 Overlap을 체크한다.
	bool SectorOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InDegree, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams(), bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
	bool SectorOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InDegree, const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& InResponseParams = FCollisionResponseParams::DefaultResponseParam, bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);

	// 대략적인 Overlap 위치를 계산.
	bool GetRoughOverlapPosition( const FVector& InOrigin, const UPrimitiveComponent* InOtherComp, FVector& OutPos );
};
