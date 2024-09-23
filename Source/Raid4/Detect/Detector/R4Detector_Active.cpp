// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_Active.h"
#include "../R4DetectStruct.h"

#include <Components/ShapeComponent.h> 
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Detector_Active)

AR4Detector_Active::AR4Detector_Active()
{
	PrimaryActorTick.bCanEverTick = false;
	bRelevantForLevelBounds = false;
	SetCanBeDamaged( false );

	// 혹시나 Replicate 시 위치를 제대로 Replicate 하기 위함
	SetReplicatingMovement( true );
}

void AR4Detector_Active::BeginPlay()
{
	Super::BeginPlay();

	// Find ShapeComps
	TInlineComponentArray<UShapeComponent*> shapeComps;
	GetComponents<UShapeComponent>( shapeComps, true );
	for ( auto& shapeComp : shapeComps )
	{
		// Caching
		CachedShapeComp.Add( {shapeComp, shapeComp->GetCollisionEnabled()} );
		
		// 멤버로 추가된 Shape Component를 모두 찾아서 한개의 Delegate로 나가도록 연결
		shapeComp->OnComponentBeginOverlap.AddDynamic( this, &AR4Detector_Active::_OnBeginShapeOverlap );
		shapeComp->OnComponentEndOverlap.AddDynamic( this, &AR4Detector_Active::_OnEndShapeOverlap );
	}
}

void AR4Detector_Active::PreReturnPoolObject()
{
	Super::PreReturnPoolObject();
	TearDownDetect();
}

/**
 *	Detect 준비
 *	@param InOrigin : 탐지의 기준이 되는 Transform
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_Active::SetupDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc )
{
	// Rotation & Position
	// InOrigin에 Relative Location을 더한 위치, 회전을 설정.
	SetActorRotation( InOrigin.TransformRotation( InDetectDesc.RelativeRot.Quaternion() ) );
	SetActorLocation( InOrigin.TransformPosition( InDetectDesc.RelativeLoc ) );

	// 생명 주기 설정
	// 아주 작으면 다음 틱에 바로 반납
	if ( InDetectDesc.LifeTime <= KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<AR4Detector_Active>(this)]
		{
			// object pool에 자신을 반납.
			if(thisPtr.IsValid())
				OBJECT_POOL->ReturnPoolObject( thisPtr.Get() );
		} );
	}
	else
	{
		// 아닌 경우 생명주기 타이머 설정.
		GetWorldTimerManager().SetTimer( LifeTimerHandle,
			[thisPtr = TWeakObjectPtr<AR4Detector_Active>(this)]
			{
				// object pool에 자신을 반납.
				if(thisPtr.IsValid())
					OBJECT_POOL->ReturnPoolObject( thisPtr.Get() );
			}, InDetectDesc.LifeTime, false );
	}

	BP_SetupDetect( InOrigin, InDetectDesc );
}

/**
 *	Detect 정리
 */
void AR4Detector_Active::TearDownDetect()
{
	BP_TearDownDetect();
	
	// disable collision
	for ( auto& [shapeComp, collisionType] : CachedShapeComp )
	{
		if(!shapeComp.IsValid())
		{
			LOG_ERROR(R4Log, TEXT("Member Shape Comp is invalid."));
			continue;
		}
		
		shapeComp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}

	// Attach해서 사용되었다면 Detach
	if ( GetAttachParentActor() )
		DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );
	
	OnBeginDetectDelegate.Clear();
	OnEndDetectDelegate.Clear();
	GetWorldTimerManager().ClearTimer( LifeTimerHandle );
}

/**
 *	Detect 실행
 */
void AR4Detector_Active::ExecuteDetect()
{
	// Collision Enable
	for ( auto& [shapeComp, collisionType] : CachedShapeComp )
	{
		if(!shapeComp.IsValid())
		{
			LOG_ERROR(R4Log, TEXT("Member Shape Comp is invalid."));
			continue;
		}
		
		shapeComp->SetCollisionEnabled( collisionType );
	}
}

/**
 *	멤버로 등록된 Shape Comp Begin Overlap 시 호출
 */
void AR4Detector_Active::_OnBeginShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	FR4DetectResult result;
	result.DetectedActor = OtherActor;
	result.DetectedComponent = OtherComp;
	// TODO: Trace로 대략적 위치 계싼 할까? 말까?
	result.Location = SweepResult.Location;

	if ( OnBeginDetectDelegate.IsBound() )
		OnBeginDetectDelegate.Broadcast( result );
}

/**
 *	멤버로 등록된 Shape Comp End Overlap 시 호출
 */ 
void AR4Detector_Active::_OnEndShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	FR4DetectResult result;
	result.DetectedActor = OtherActor;
	result.DetectedComponent = OtherComp;
	if ( IsValid( OverlappedComponent ) )
		result.Location = OverlappedComponent->GetComponentLocation();

	if ( OnEndDetectDelegate.IsBound() )
		OnEndDetectDelegate.Broadcast( result );
}
