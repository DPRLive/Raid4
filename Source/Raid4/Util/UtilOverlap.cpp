#include "UtilOverlap.h"
#include "../Helper/DrawDebugHelper.h"

#include <WorldCollision.h>
#include <DrawDebugHelpers.h>
#include <Components/PrimitiveComponent.h>
#include <Engine/OverlapResult.h>

/**
* Box 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Box의 중심
* @param InRot : Box의 회전
* @param InBoxHalfExtent : BoxExtent
* @param InProfileName : Overlap 체크에 사용할 Collision Profile
* @param InParams : Overlap 체크 시 사용할 추가 Param
*/
bool UtilOverlap::BoxOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, const FVector& InBoxHalfExtent, const FName InProfileName, const FCollisionQueryParams& InParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByProfile(OutOverlaps, InLoc, InRot, InProfileName, FCollisionShape::MakeBox(InBoxHalfExtent), InParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugBox(InWorld, InLoc, InBoxHalfExtent, InRot, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Box 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Box의 중심
* @param InRot : Box의 회전
* @param InBoxHalfExtent : BoxExtent
* @param InTraceChannel : Overlap 체크에 사용할 Collision Channel
* @param InParams : Overlap 체크 시 사용할 추가 Param
* @param InResponseParams : Overlap 체크 시 사용할 추가 ResponseParam
*/
bool UtilOverlap::BoxOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc,
	const FQuat& InRot, const FVector& InBoxHalfExtent, const ECollisionChannel InTraceChannel,
	const FCollisionQueryParams& InParams, const FCollisionResponseParams& InResponseParams, bool InIsShowDebug,
	const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByChannel(OutOverlaps, InLoc, InRot, InTraceChannel, FCollisionShape::MakeBox(InBoxHalfExtent), InParams, InResponseParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugBox(InWorld, InLoc, InBoxHalfExtent, InRot, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Sphere 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Sphere의 중심
* @param InRadius : Sphere의 반지름
* @param InProfileName : Overlap 체크에 사용할 Collision Profile
* @param InParams : Overlap 체크 시 사용할 추가 Param
*/
bool UtilOverlap::SphereOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, float InRadius, const FName InProfileName, const FCollisionQueryParams& InParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByProfile(OutOverlaps, InLoc, FQuat::Identity, InProfileName, FCollisionShape::MakeSphere(InRadius), InParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugSphere(InWorld, InLoc, InRadius, 32, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Sphere 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Sphere의 중심
* @param InRadius : Sphere의 반지름
* @param InTraceChannel : Overlap 체크에 사용할 Collision Channel
* @param InParams : Overlap 체크 시 사용할 추가 Param
* @param InResponseParams : Overlap 체크 시 사용할 추가 ResponseParam
*/
bool UtilOverlap::SphereOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld,
	const FVector& InLoc, float InRadius, const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams,
	const FCollisionResponseParams& InResponseParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByChannel(OutOverlaps, InLoc, FQuat::Identity, InTraceChannel, FCollisionShape::MakeSphere(InRadius), InParams, InResponseParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugSphere(InWorld, InLoc, InRadius, 32, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Capsule 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Capsule의 중심
* @param InRot : Capsule의 회전
* @param InRadius : Capsule의 반지름
* @param InHalfHeight : Capsule의 반 높이
* @param InProfileName : Overlap 체크에 사용할 Collision Profile
* @param InParams : Overlap 체크 시 사용할 추가 Param
*/
bool UtilOverlap::CapsuleOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FVector& InLoc, const FQuat& InRot, float InRadius, float InHalfHeight, const FName InProfileName, const FCollisionQueryParams& InParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByProfile(OutOverlaps, InLoc, FQuat::Identity, InProfileName, FCollisionShape::MakeCapsule(InRadius, InHalfHeight), InParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugCapsule(InWorld, InLoc, InHalfHeight, InRadius, InRot, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Capsule 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InLoc : Capsule의 중심
* @param InRot : Capsule의 회전
* @param InRadius : Capsule의 반지름
* @param InHalfHeight : Capsule의 반 높이
* @param InTraceChannel : Overlap 체크에 사용할 Collision Channel
* @param InParams : Overlap 체크 시 사용할 추가 Param
* @param InResponseParams : Overlap 체크 시 사용할 추가 ResponseParam
*/
bool UtilOverlap::CapsuleOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld,
	const FVector& InLoc, const FQuat& InRot, float InRadius, float InHalfHeight,
	const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams,
	const FCollisionResponseParams& InResponseParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	bool bOverlap = IsValid(InWorld) ? InWorld->OverlapMultiByChannel(OutOverlaps, InLoc, FQuat::Identity, InTraceChannel, FCollisionShape::MakeCapsule(InRadius, InHalfHeight), InParams, InResponseParams) : false;

#if UE_ENABLE_DEBUG_DRAWING
	if(InWorld && InIsShowDebug)
		DrawDebugCapsule(InWorld, InLoc, InHalfHeight, InRadius, InRot, InDebugColor, false, InDebugTime);
#endif

	return bOverlap;
}

/**
* Sector (부채꼴) 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InTrans : Sector의 Transform
* @param InRadius : Sector의 반지름
* @param InHalfHeight : Sector의 반 높이
* @param InDegree : Sector의 내부 각도 (Degree)
* @param InProfileName : Overlap 체크에 사용할 Collision Profile
* @param InParams : Overlap 체크 시 사용할 추가 Param
*/
bool UtilOverlap::SectorOverlapByProfile(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InDegree, const FName InProfileName, const FCollisionQueryParams& InParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
	if(!IsValid(InWorld))
		return false;

	// 각도 0~360으로 제한
	InDegree = FMath::Clamp(InDegree, 0.f, 360.f);
	const float halfDegree = InDegree / 2; 
	
	// 부채꼴의 좌, 우 면을 먼저 체크한다.
	TSet<FActorInstanceHandle> planeOverlaps;
	{
		TArray<FOverlapResult> planeOverlapArr;

		// 시계 방향 쪽 평면 체크
		FTransform planeTrans;
		planeTrans.SetRotation(InTrans.TransformRotation(FRotator(0.f, halfDegree, 0.f).Quaternion()));
		planeTrans.SetLocation(InTrans.GetLocation() + planeTrans.GetUnitAxis(EAxis::X) * InRadius / 2);
		BoxOverlapByProfile(planeOverlapArr, InWorld, planeTrans.GetLocation(), planeTrans.GetRotation(), FVector(InRadius / 2, 0.f, InHalfHeight), InProfileName, InParams, InIsShowDebug, FColor::Silver, InDebugTime);
		
		for(FOverlapResult& res : planeOverlapArr)
			planeOverlaps.Emplace(MoveTemp(res.OverlapObjectHandle));

		planeOverlapArr.Empty();
		
		// 반 시계 방향 쪽 평면 체크
		planeTrans.SetRotation(InTrans.TransformRotation(FRotator(0.f, -halfDegree, 0.f).Quaternion()));
		planeTrans.SetLocation(InTrans.GetLocation() + planeTrans.GetUnitAxis(EAxis::X) * InRadius / 2);
		BoxOverlapByProfile(planeOverlapArr, InWorld, planeTrans.GetLocation(), planeTrans.GetRotation(), FVector(InRadius / 2, 0.f, InHalfHeight), InProfileName, InParams, InIsShowDebug, FColor::Silver, InDebugTime);

		for(FOverlapResult& res : planeOverlapArr)
			planeOverlaps.Emplace(MoveTemp(res.OverlapObjectHandle));
	}
	
	// 부채꼴의 반지름과 높이로 만든 박스로 체크한다.
	TArray<FOverlapResult> boxOverlaps;
	BoxOverlapByProfile(boxOverlaps, InWorld, InTrans.GetLocation(), InTrans.GetRotation(), FVector(InRadius, InRadius, InHalfHeight), InProfileName, InParams, InIsShowDebug, FColor::Silver, InDebugTime);

	// 박스에 체크 된 것들 거르기
	for(FOverlapResult& overlapResult : boxOverlaps)
	{
		// 양 평면에 체크된 경우 -> 이미 오버랩으로 판정
		if(planeOverlaps.Find(overlapResult.OverlapObjectHandle))
		{
			OutOverlaps.Emplace(MoveTemp(overlapResult));
			continue;
		}

		FVector closestPoint;
		overlapResult.GetComponent()->GetClosestPointOnCollision(InTrans.GetLocation(), closestPoint);
		const FVector centerToTarget = closestPoint - InTrans.GetLocation();
		const FVector projVector = FVector::VectorPlaneProject(centerToTarget, InTrans.GetUnitAxis(EAxis::Z)); // 높이는 이미 체크가 되었으니 centerToTarget을 부채꼴이 속한 평면에 정사영 
		float dot = FVector::DotProduct(projVector.GetSafeNormal(), InTrans.GetUnitAxis(EAxis::X)); // 내적으로 Cos 값을 구함

#if UE_ENABLE_DEBUG_DRAWING
		if( IsValid(InWorld) && InIsShowDebug )
		{
			DrawDebugPoint(InWorld, closestPoint, 3, InDebugColor, false, InDebugTime);
			DrawDebugLine(InWorld, InTrans.GetLocation(), InTrans.GetLocation() + projVector, InDebugColor, false, InDebugTime, 0, 2);
		}
#endif
		
		// 각 안에 들어 오는가? 내적(Cos) 값 (1 ~ -1)이 더 작으면 안들어 온 것 && 반지름 안에 들어오는가?
		if((dot < FMath::Cos(FMath::DegreesToRadians(halfDegree))) || (projVector.Size() > InRadius))
			continue;
		
		OutOverlaps.Emplace(MoveTemp(overlapResult));
	}

#if UE_ENABLE_DEBUG_DRAWING
	if( IsValid(InWorld) && InIsShowDebug )
	{
		DrawDebugHelper::DrawDebugSector(InWorld, InTrans, InRadius, InHalfHeight, FMath::DegreesToRadians(InDegree), 32, InDebugColor, false, InDebugTime);
	}
#endif

	return true;
}

/**
* Sector (부채꼴) 형태의 Overlap을 체크한다.
* @param OutOverlaps : 결과를 받아갈 배열
* @param InWorld : 실행할 월드
* @param InTrans : Sector의 Transform
* @param InRadius : Sector의 반지름
* @param InHalfHeight : Sector의 반 높이
* @param InDegree : Sector의 내부 각도 (Degree)
* @param InTraceChannel : Overlap 체크에 사용할 Collision Channel
* @param InParams : Overlap 체크 시 사용할 추가 Param
* @param InResponseParams : Overlap 체크 시 사용할 추가 ResponseParam
*/
bool UtilOverlap::SectorOverlapByChannel(TArray<FOverlapResult>& OutOverlaps, const UWorld* InWorld,
	const FTransform& InTrans, float InRadius, float InHalfHeight, float InDegree,
	const ECollisionChannel InTraceChannel, const FCollisionQueryParams& InParams,
	const FCollisionResponseParams& InResponseParams, bool InIsShowDebug, const FColor& InDebugColor, float InDebugTime)
{
		if(!IsValid(InWorld))
		return false;

	// 각도 0~360으로 제한
	InDegree = FMath::Clamp(InDegree, 0.f, 360.f);
	const float halfDegree = InDegree / 2; 
	
	// 부채꼴의 좌, 우 면을 먼저 체크한다.
	TSet<FActorInstanceHandle> planeOverlaps;
	{
		TArray<FOverlapResult> planeOverlapArr;

		// 시계 방향 쪽 평면 체크
		FTransform planeTrans;
		planeTrans.SetRotation(InTrans.TransformRotation(FRotator(0.f, halfDegree, 0.f).Quaternion()));
		planeTrans.SetLocation(InTrans.GetLocation() + planeTrans.GetUnitAxis(EAxis::X) * InRadius / 2);
		BoxOverlapByChannel(planeOverlapArr, InWorld, planeTrans.GetLocation(), planeTrans.GetRotation(), FVector(InRadius / 2, 0.f, InHalfHeight), InTraceChannel, InParams, InResponseParams, InIsShowDebug, FColor::Silver, InDebugTime);
		
		for(FOverlapResult& res : planeOverlapArr)
			planeOverlaps.Emplace(MoveTemp(res.OverlapObjectHandle));

		planeOverlapArr.Empty();
		
		// 반 시계 방향 쪽 평면 체크
		planeTrans.SetRotation(InTrans.TransformRotation(FRotator(0.f, -halfDegree, 0.f).Quaternion()));
		planeTrans.SetLocation(InTrans.GetLocation() + planeTrans.GetUnitAxis(EAxis::X) * InRadius / 2);
		BoxOverlapByChannel(planeOverlapArr, InWorld, planeTrans.GetLocation(), planeTrans.GetRotation(), FVector(InRadius / 2, 0.f, InHalfHeight), InTraceChannel, InParams, InResponseParams, InIsShowDebug, FColor::Silver, InDebugTime);

		for(FOverlapResult& res : planeOverlapArr)
			planeOverlaps.Emplace(MoveTemp(res.OverlapObjectHandle));
	}
	
	// 부채꼴의 반지름과 높이로 만든 박스로 체크한다.
	TArray<FOverlapResult> boxOverlaps;
	BoxOverlapByChannel(boxOverlaps, InWorld, InTrans.GetLocation(), InTrans.GetRotation(), FVector(InRadius, InRadius, InHalfHeight), InTraceChannel, InParams, InResponseParams, InIsShowDebug, FColor::Silver, InDebugTime);

	// 박스에 체크 된 것들 거르기
	for(FOverlapResult& overlapResult : boxOverlaps)
	{
		// 양 평면에 체크된 경우 -> 이미 오버랩으로 판정
		if(planeOverlaps.Find(overlapResult.OverlapObjectHandle))
		{
			OutOverlaps.Emplace(MoveTemp(overlapResult));
			continue;
		}

		FVector closestPoint;
		overlapResult.GetComponent()->GetClosestPointOnCollision(InTrans.GetLocation(), closestPoint);
		const FVector centerToTarget = closestPoint - InTrans.GetLocation();
		const FVector projVector = FVector::VectorPlaneProject(centerToTarget, InTrans.GetUnitAxis(EAxis::Z)); // 높이는 이미 체크가 되었으니 centerToTarget을 부채꼴이 속한 평면에 정사영 
		float dot = FVector::DotProduct(projVector.GetSafeNormal(), InTrans.GetUnitAxis(EAxis::X)); // 내적으로 Cos 값을 구함

#if UE_ENABLE_DEBUG_DRAWING
		if( IsValid(InWorld) && InIsShowDebug )
		{
			DrawDebugPoint(InWorld, closestPoint, 3, InDebugColor, false, InDebugTime);
			DrawDebugLine(InWorld, InTrans.GetLocation(), InTrans.GetLocation() + projVector, InDebugColor, false, InDebugTime, 0, 2);
		}
#endif
		
		// 각 안에 들어 오는가? 내적(Cos) 값 (1 ~ -1)이 더 작으면 안들어 온 것 && 반지름 안에 들어오는가?
		if((dot < FMath::Cos(FMath::DegreesToRadians(halfDegree))) || (projVector.Size() > InRadius))
			continue;
		
		OutOverlaps.Emplace(MoveTemp(overlapResult));
	}

#if UE_ENABLE_DEBUG_DRAWING
	if( IsValid(InWorld) && InIsShowDebug )
	{
		DrawDebugHelper::DrawDebugSector(InWorld, InTrans, InRadius, InHalfHeight, FMath::DegreesToRadians(InDegree), 32, InDebugColor, false, InDebugTime);
	}
#endif

	return true;
}
