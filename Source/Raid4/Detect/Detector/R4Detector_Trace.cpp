// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_Trace.h"
#include "../R4DetectStruct.h"
#include "../../Util/UtilOverlap.h"

#include <Net/UnrealNetwork.h>
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
	
	// 기본적으로 No Authority 환경에서도 같이 Detect를 진행.
	bDetectOnNoAuthority = true;
	NoAuthTraceEnableInfo = FR4NoAuthDetectEnableInfo();
	
	bDrawDebug = true;
	DebugTime = 1.f;
	DebugColor = FColor::Green;
}

void AR4Detector_Trace::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( AR4Detector_Trace, NoAuthTraceEnableInfo );
	DOREPLIFETIME( AR4Detector_Trace, CachedRequestActor );
}

void AR4Detector_Trace::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	_TearDownDetect();
	Super::EndPlay( EndPlayReason );
}

void AR4Detector_Trace::PreReturnPoolObject()
{
	_TearDownDetect();
	Super::PreReturnPoolObject();
}

/**
 *	Detect 준비
 *  @param InRequestActor : Detect를 요청한 AActor.
 *	@param InOrigin : 탐지의 기준이 되는 Transform
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_Trace::ExecuteDetect( AActor* InRequestActor, const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc )
{
	CachedRequestActor = InRequestActor;
	
	// Rotation & Position
	// InOrigin에 Relative Location을 더한 위치, 회전을 설정.
	SetActorRotation( InOrigin.TransformRotation( InDetectDesc.RelativeRot.Quaternion() ) );
	SetActorLocation( InOrigin.TransformPosition( InDetectDesc.RelativeLoc ) );
	
	// 생명 주기 설정
	_SetLifeTime( InDetectDesc.LifeTime );

	// Trace 실행
	_ExecuteTrace( 0.f );

	// NoAuthority 경우에도 Trace가 필요한 경우, 정보를 Replicate
	if( HasAuthority() && bDetectOnNoAuthority )
	{
		NoAuthTraceEnableInfo.bEnable = true;
		NoAuthTraceEnableInfo.EnableServerTime = R4GetServerTimeSeconds( GetWorld() );
		NoAuthTraceEnableInfo.LifeTime = InDetectDesc.LifeTime;
	}
	
	BP_ExecuteDetect( InRequestActor, InOrigin, InDetectDesc );
}

/**
 *	Life Time 설정
 *	@param InLifeTime : 생명 주기
 */
void AR4Detector_Trace::_SetLifeTime( float InLifeTime )
{
	// 아주 작으면 다음 틱에 바로 반납
	if ( InLifeTime < KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<AR4Detector_Trace>(this)]
		{
			// object pool에 자신을 반납.
			if(thisPtr.IsValid())
			{
				if ( thisPtr->HasAuthority() )			
					OBJECT_POOL(thisPtr->GetWorld())->ReturnPoolObject( thisPtr.Get() );
				else
					thisPtr->_TearDownDetect();
			}
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
			{
				if ( thisPtr->HasAuthority() )			
					OBJECT_POOL(thisPtr->GetWorld())->ReturnPoolObject( thisPtr.Get() );
				else
					thisPtr->_TearDownDetect();
			}
		}, FMath::Max( InLifeTime - KINDA_SMALL_NUMBER, KINDA_SMALL_NUMBER ), false );
		// Interval과 Life Time이 동일할 시 마지막 호출 방지, PlayRate 0 이하 방지
	}
}

/**
 *	Trace를 Delay에 맞춰 예약
 *	@param InEnableDelayedTime : Trace가 Enable 된 시점으로부터 delay된 시간
 */
void AR4Detector_Trace::_ExecuteTrace( float InEnableDelayedTime )
{
	// OneShot
	if( ExecutionType == ER4TraceDetectExecutionType::OneShot )
	{	
		// Enable Delay가 Delay보다 많이 지난 경우, Skip
		if( InEnableDelayedTime - Delay > KINDA_SMALL_NUMBER )
			return;

		float delay = Delay - InEnableDelayedTime;

		// delay가 아주 작으면 바로 실행
		if ( delay < KINDA_SMALL_NUMBER )
		{
			_Trace();
			return;
		}
		
		// Delay 후 실행
		GetWorldTimerManager().SetTimer( TraceTimerHandle,
		[thisPtr = TWeakObjectPtr<AR4Detector_Trace>( this )]( )
		{
			 if ( thisPtr.IsValid() )
			 	thisPtr->_Trace();
		}, delay, false );
		
		return;
	}

	// Interval
	if ( ExecutionType == ER4TraceDetectExecutionType::Interval
		&& (DetectInterval > KINDA_SMALL_NUMBER) )
	{
		float firstDelay = -1.f;
		
		// Enable Delay가 Delay보다 적게 지난경우
		if( Delay + KINDA_SMALL_NUMBER > InEnableDelayedTime )
			firstDelay = Delay - InEnableDelayedTime;
		else
		{
			// Enable Delay가 더 많이 지난 경우, 이미 실행해야 했을 Interval들 Skip
			firstDelay = InEnableDelayedTime - Delay;
			firstDelay -= FMath::FloorToInt( firstDelay / DetectInterval ) * DetectInterval;

			// 다음 실행 전까지의 Delay 찾기
			firstDelay = DetectInterval - firstDelay;
		}

		// first Delay가 아주 작은경우, 바로 한번 시작 후 first Delay를 Interval로 되도록 설정
		if ( firstDelay < KINDA_SMALL_NUMBER )
		{
			_Trace();
			firstDelay = -1.f;
		}

		// Interval Time 마다 실행
		GetWorldTimerManager().SetTimer( TraceTimerHandle,
		 [thisPtr = TWeakObjectPtr<AR4Detector_Trace>( this )]( )
		 {
			 if ( thisPtr.IsValid() )
			 	thisPtr->_Trace( );
		 }, DetectInterval, true, firstDelay );
	}
}

/**
 *	Trace Logic
 */
void AR4Detector_Trace::_Trace( )
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
		case ER4TraceDetectShapeType::Box:
			UtilOverlap::BoxOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.BoxHalfExtent, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4TraceDetectShapeType::Sphere:
			UtilOverlap::SphereOverlapByChannel(overlapResults, world, center, ShapeInfo.Radius, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4TraceDetectShapeType::Capsule:
			UtilOverlap::CapsuleOverlapByChannel(overlapResults, world, center, quat, ShapeInfo.Radius, ShapeInfo.HalfHeight, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case ER4TraceDetectShapeType::Sector:
			// meshTrans.GetUnitAxis(EAxis::X)는 origin의 로컬 X축을 구함
			UtilOverlap::SectorOverlapByChannel(overlapResults, world, FTransform(quat, center), ShapeInfo.Radius, ShapeInfo.HalfHeight, ShapeInfo.Angle, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		}
		
		// 결과를 처리
		for(const auto& overlapResult : overlapResults)
		{
			FR4DetectResult result;
			result.RequestActor = CachedRequestActor;
			result.Detector = this;
			result.DetectedActor = overlapResult.GetActor();
			result.DetectedComponent = overlapResult.GetComponent();

			// 대략적인 위치 계산.
			UtilOverlap::GetRoughOverlapPosition( center, overlapResult.GetComponent(), result.Location );
			
			// OneFrame 의 경우에는 begin, end가 동시에 발생
			if ( OnBeginDetectDelegate.IsBound() )
				OnBeginDetectDelegate.Broadcast( result );

			BP_OnBeginDetect( result );

			if ( OnEndDetectDelegate.IsBound() )
				OnEndDetectDelegate.Broadcast( result );
		}
	}
}

/**
 *	Detect 정리
 */
void AR4Detector_Trace::_TearDownDetect()
{
	if ( HasAuthority() )
	{
		BP_TearDownDetect();

		// Attach해서 사용되었다면 Detach
		if ( GetAttachParentActor() )
			DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );

		// disable Trace
		NoAuthTraceEnableInfo.bEnable = false;
	
		OnBeginDetectDelegate.Clear();
		OnEndDetectDelegate.Clear();
	}

	CachedRequestActor.Reset();
	GetWorldTimerManager().ClearTimer( LifeTimerHandle );
	GetWorldTimerManager().ClearTimer( TraceTimerHandle );
}

void AR4Detector_Trace::_OnRep_NoAuthTraceInfo()
{
	// Trace를 실행
	if ( NoAuthTraceEnableInfo.bEnable )
	{
		// 생명주기 설정
		_SetLifeTime( NoAuthTraceEnableInfo.LifeTime );
		
		// 서버 enable 시작으로부터 지연된 시간
		float enableDelayedTime = FMath::Max( 0.f, R4GetServerTimeSeconds( GetWorld() ) - NoAuthTraceEnableInfo.EnableServerTime );

		// Detect 실행
		_ExecuteTrace( enableDelayedTime );

		return;
	}

	_TearDownDetect( );
}
