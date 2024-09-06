// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_OneFrame.h"

#include "../R4DetectStruct.h"
#include "../../Util/UtilOverlap.h"

#include <Engine/OverlapResult.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Detector_OneFrame)

AR4Detector_OneFrame::AR4Detector_OneFrame()
{
	PrimaryActorTick.bCanEverTick = false;
	bRelevantForLevelBounds = false;
	SetCanBeDamaged(false);

	// Local에서 체크할 예정
	SetReplicates(false);
	
	bDrawDebug = true;
	DebugTime = 1.f;
	DebugColor = FColor::Green;
}

void AR4Detector_OneFrame::BeginPlay()
{
	Super::BeginPlay();
}

void AR4Detector_OneFrame::PreReturnPoolObject()
{
	Super::PreReturnPoolObject();

	OnBeginDetectDelegate.Clear();
	OnEndDetectDelegate.Clear();
}

/**
 *	Detect 실행
 *	@param InInstigator : 탐지를 요청한 객체
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_OneFrame::ExecuteDetect( const AActor* InInstigator, const FR4DetectDesc& InDetectDesc )
{
	if (const UWorld* world = GetWorld(); IsValid(world))
	{
		// Relative Location을 더한 위치, 회전을 구함
		const FTransform& originTrans = InInstigator->GetActorTransform();
		const FVector center = originTrans.TransformPosition(InDetectDesc.RelativeLoc);
		const FQuat quat = originTrans.GetRotation() * InDetectDesc.RelativeRot.Quaternion();

		FCollisionQueryParams params;
		TArray<FOverlapResult> overlapResults;

		// Collision Channel 설정
		FCollisionResponseParams responseParams(InDetectDesc.BodyInstance.GetResponseToChannels());
		
		//BodyInstance.
		switch ( ShapeInfo.Shape ) // 모양에 맞춰 overlap 체크
		{
		case ER4DetectShapeType::Box:
			UtilOverlap::BoxOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.BoxHalfExtent, InDetectDesc.BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Sphere:
			UtilOverlap::SphereOverlapByChannel(overlapResults, world, center, ShapeInfo.Radius, InDetectDesc.BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Capsule:
			UtilOverlap::CapsuleOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.Radius, ShapeInfo.HalfHeight, InDetectDesc.BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Sector:
			// meshTrans.GetUnitAxis(EAxis::X) 는 MeshComp의 로컬 X축을 구함
			UtilOverlap::SectorOverlapByChannel(overlapResults, world, FTransform(quat, center), ShapeInfo.Radius, ShapeInfo.HalfHeight, ShapeInfo.Angle, InDetectDesc.BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		}
		
		// 결과를 처리
		for(const auto& overlapResult : overlapResults)
		{
			FR4DetectResult result;
			
			// Location : closest point
			overlapResult.GetComponent()->GetClosestPointOnCollision(center, result.Location);
			result.DetectedActor = overlapResult.GetActor();
			result.DetectedComponent = overlapResult.GetComponent();

			// OneFrame 의 경우에는 begin, end가 동시에 발생
			if(OnBeginDetectDelegate.IsBound())
				OnBeginDetectDelegate.Broadcast(result);

			if(OnEndDetectDelegate.IsBound())
				OnEndDetectDelegate.Broadcast(result);
		}
	}

	// object pool에 자신을 반납.
	OBJECT_POOL->ReturnPoolObject( this );
}