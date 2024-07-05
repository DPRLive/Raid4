#pragma once

/**
 * Overlap을 도와주는 Overlap helper
 */
namespace UtilOverlap
{
	/**
	 * Box 형태의 Overlap을 체크한다.
	 */
	bool BoxOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, const FVector& InBoxHalfExtent, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams(), bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);

	/**
	 * Sphere 형태의 Overlap을 체크한다.
	 */
	bool SphereOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, float InRadius, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams(), bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);

	/**
	 * Capsule 형태의 Overlap을 체크한다.
	 */
	bool CapsuleOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, float InRadius, float InHalfHeight, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams(), bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);

	/**
	 * Sector (부채꼴) 형태의 Overlap을 체크한다.
	 */
	bool SectorOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InDegree, const FName InProfileName, const FCollisionQueryParams& InParams = FCollisionQueryParams(), bool InIsShowDebug = false, const FColor& InDebugColor = FColor::Red, float InDebugTime = 0.f);
};
