// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_Trace.h"
#include "../R4DetectStruct.h"
#include "../../Util/UtilOverlap.h"

#include <Engine/OverlapResult.h>
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Detector_Trace)

AR4Detector_Trace::AR4Detector_Trace()
{
	PrimaryActorTick.bCanEverTick = false;
	bRelevantForLevelBounds = false;
	SetCanBeDamaged( false );

	// 혹시나 Replicate 시 위치를 제대로 Replicate 하기 위함
	SetReplicatingMovement( true );

	// Actor Pool에서 자동으로 Collision 변경하지 못하도록 설정
	bControlCollisionByPool = false;
	SetActorEnableCollision( false );

	bDrawDebug = true;
	DebugTime = 1.f;
	DebugColor = FColor::Green;
}

void AR4Detector_Trace::BeginPlay()
{
	Super::BeginPlay();
}

void AR4Detector_Trace::PostInitPoolObject()
{
	Super::PostInitPoolObject();
}

void AR4Detector_Trace::PreReturnPoolObject()
{
	Super::PreReturnPoolObject();
	TearDownDetect();
}

/**
 *	Detect 준비
 *	@param InOrigin : 탐지의 기준이 되는 Transform
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_Trace::SetupDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc )
{
	// Rotation & Position
	// InOrigin에 Relative Location을 더한 위치, 회전을 설정.
	SetActorRotation( InOrigin.TransformRotation( InDetectDesc.RelativeRot.Quaternion() ) );
	SetActorLocation( InOrigin.TransformPosition( InDetectDesc.RelativeLoc ) );
	
	// 생명 주기 설정
	// 아주 작으면 다음 틱에 바로 반납
	if ( InDetectDesc.LifeTime <= KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<AR4Detector_Trace>(this)]
		{
			// object pool에 자신을 반납.
			if(thisPtr.IsValid())
				OBJECT_POOL(thisPtr->GetWorld())->ReturnPoolObject( thisPtr.Get() );
		} );
	}
	else
	{
		// 아닌 경우 생명주기 타이머 설정.
		GetWorldTimerManager().SetTimer( LifeTimerHandle,
		[thisPtr = TWeakObjectPtr<AR4Detector_Trace>(this)]
		{
			// object pool에 자신을 반납.
			if(thisPtr.IsValid())
				OBJECT_POOL(thisPtr->GetWorld())->ReturnPoolObject( thisPtr.Get() );
		}, InDetectDesc.LifeTime - KINDA_SMALL_NUMBER, false );
	}

	BP_SetupDetect( InOrigin, InDetectDesc );
}

/**
 *	Detect 정리
 */
void AR4Detector_Trace::TearDownDetect()
{
	BP_TearDownDetect();

	// Attach해서 사용되었다면 Detach
	if ( GetAttachParentActor() )
		DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );

	// disable collision
	SetActorEnableCollision( false );
	
	OnBeginDetectDelegate.Clear();
	OnEndDetectDelegate.Clear();
	GetWorldTimerManager().ClearTimer( LifeTimerHandle );
	GetWorldTimerManager().ClearTimer( IntervalTimerHandle );
}

/**
 *	Detect 실행
 */
void AR4Detector_Trace::ExecuteDetect( )
{
	// Collision Enable
	SetActorEnableCollision( true );
	
	// 1회 실행
	_Trace( );

	// Interval 설정의 경우 Interval마다 실행.
	if( DetectInterval > KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimer( IntervalTimerHandle,
		 [thisPtr = TWeakObjectPtr<AR4Detector_Trace>( this )]( )
		 {
			 if ( thisPtr.IsValid() )
			 	thisPtr->_Trace( );
		 }, DetectInterval, true );
	}
}

/**
 *	Trace 실행
 */
void AR4Detector_Trace::_Trace( ) const
{
	if (const UWorld* world = GetWorld(); IsValid(world))
	{
		const FVector center = GetActorLocation();
		const FQuat quat = GetActorQuat();

		FCollisionQueryParams params;
		TArray<FOverlapResult> overlapResults;

		// Collision Channel 설정
		FCollisionResponseParams responseParams(BodyInstance.GetResponseToChannels());
		
		//BodyInstance.
		switch ( ShapeInfo.Shape ) // 모양에 맞춰 overlap 체크
		{
		case ER4DetectShapeType::Box:
			UtilOverlap::BoxOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.BoxHalfExtent, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Sphere:
			UtilOverlap::SphereOverlapByChannel(overlapResults, world, center, ShapeInfo.Radius, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Capsule:
			UtilOverlap::CapsuleOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.Radius, ShapeInfo.HalfHeight, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4DetectShapeType::Sector:
			// meshTrans.GetUnitAxis(EAxis::X)는 origin의 로컬 X축을 구함
			UtilOverlap::SectorOverlapByChannel(overlapResults, world, FTransform(quat, center), ShapeInfo.Radius, ShapeInfo.HalfHeight, ShapeInfo.Angle, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
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
}
