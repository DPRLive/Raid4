// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_Active.h"
#include "../R4DetectStruct.h"
#include "../../Util/UtilOverlap.h"

#include <Net/UnrealNetwork.h>
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

	// Actor Pool에서 자동으로 Collision 변경하지 못하도록 설정
	bControlCollisionByPool = false;
	SetActorEnableCollision( false );

	// 기본적으로 No Authority 환경에서도 같이 Detect를 진행.
	bDetectOnNoAuthority = true;
	NoAuthCollisionEnableInfo = FR4NoAuthDetectEnableInfo();
}

void AR4Detector_Active::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( AR4Detector_Active, NoAuthCollisionEnableInfo );
	DOREPLIFETIME( AR4Detector_Active, CachedRequestActor );
}

void AR4Detector_Active::BeginPlay()
{
	Super::BeginPlay();

	// Find ShapeComps
	TInlineComponentArray<UShapeComponent*> shapeComps;
	GetComponents<UShapeComponent>( shapeComps, true );
	for ( auto& shapeComp : shapeComps )
	{
		// 멤버로 추가된 Shape Component를 모두 찾아서 한개의 Delegate로 나가도록 연결
		shapeComp->OnComponentBeginOverlap.AddDynamic( this, &AR4Detector_Active::_OnBeginShapeOverlap );
		shapeComp->OnComponentEndOverlap.AddDynamic( this, &AR4Detector_Active::_OnEndShapeOverlap );
	}
}

void AR4Detector_Active::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	_TearDownDetect();
	Super::EndPlay( EndPlayReason );
}

void AR4Detector_Active::PreReturnPoolObject()
{
	_TearDownDetect();
	Super::PreReturnPoolObject();
}

/**
 *	Detect 실행
 *  @param InRequestActor : Detect를 요청한 AActor.
 *	@param InOrigin : 탐지의 기준이 되는 Transform
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_Active::ExecuteDetect( AActor* InRequestActor, const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc )
{
	CachedRequestActor = InRequestActor;
	
	// Rotation & Position
	// InOrigin에 Relative Location을 더한 위치, 회전을 설정.
	SetActorRotation( InOrigin.TransformRotation( InDetectDesc.RelativeRot.Quaternion() ) );
	SetActorLocation( InOrigin.TransformPosition( InDetectDesc.RelativeLoc ) );
	
	// Life Time 설정
	_SetLifeTime( InDetectDesc.LifeTime );

	// Collision Enable
	SetActorEnableCollision( true );

	// NoAuthority 경우에도 Collision이 필요한 경우
	if ( HasAuthority() && bDetectOnNoAuthority )
	{
		NoAuthCollisionEnableInfo.bEnable = true;
		NoAuthCollisionEnableInfo.EnableServerTime = R4GetServerTimeSeconds( GetWorld() );
		NoAuthCollisionEnableInfo.LifeTime = InDetectDesc.LifeTime;
	}
	
	BP_ExecuteDetect( InRequestActor, InOrigin, InDetectDesc );
}

/**
 *	Life Time 설정
 *	@param InLifeTime : 생명 주기
 */
void AR4Detector_Active::_SetLifeTime( float InLifeTime )
{
	// 아주 작으면 다음 틱에 바로 반납
	if ( InLifeTime <= KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<AR4Detector_Active>(this)]
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
			[thisPtr = TWeakObjectPtr<AR4Detector_Active>(this)]
			{
				/// object pool에 자신을 반납.
			if(thisPtr.IsValid())
			{
				if ( thisPtr->HasAuthority() )			
					OBJECT_POOL(thisPtr->GetWorld())->ReturnPoolObject( thisPtr.Get() );
				else
					thisPtr->_TearDownDetect();
			}
			}, FMath::Max( InLifeTime, KINDA_SMALL_NUMBER ), false );
		// PlayRate 0 방지
	}
}

/**
 *	멤버로 등록된 Shape Comp Begin Overlap 시 호출
 */
void AR4Detector_Active::_OnBeginShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	FR4DetectResult result;
	result.RequestActor = CachedRequestActor;
	result.Detector = this;
	result.DetectedActor = OtherActor;
	result.DetectedComponent = OtherComp;
	
	// 대략적인 위치 계산.
	if ( IsValid( OverlappedComponent ) )
		UtilOverlap::GetRoughOverlapPosition( OverlappedComponent->GetComponentLocation(), OtherComp, result.Location );

	if ( OnBeginDetectDelegate.IsBound() )
		OnBeginDetectDelegate.Broadcast( result );

	BP_OnBeginDetect( result );
}

/**
 *	멤버로 등록된 Shape Comp End Overlap 시 호출
 */ 
void AR4Detector_Active::_OnEndShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	FR4DetectResult result;
	result.RequestActor = CachedRequestActor;
	result.Detector = this;
	result.DetectedActor = OtherActor;
	result.DetectedComponent = OtherComp;

	// 대략적인 위치 계산.
	if ( IsValid( OverlappedComponent ) )
		UtilOverlap::GetRoughOverlapPosition( OverlappedComponent->GetComponentLocation(), OtherComp, result.Location );

	if ( OnEndDetectDelegate.IsBound() )
		OnEndDetectDelegate.Broadcast( result );

	BP_OnEndDetect( result );
}

/**
 *	Detect 정리
 */
void AR4Detector_Active::_TearDownDetect()
{
	BP_TearDownDetect();

	if( HasAuthority() )
	{
		// Attach해서 사용되었다면 Detach
		if ( GetAttachParentActor() )
			DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );

		NoAuthCollisionEnableInfo.bEnable = false;
	}

	OnBeginDetectDelegate.Clear();
	OnEndDetectDelegate.Clear();
	CachedRequestActor.Reset();

	// disable collision
	SetActorEnableCollision( false );

	// clear
	GetWorldTimerManager().ClearTimer( LifeTimerHandle );
}

void AR4Detector_Active::_OnRep_NoAuthCollisionInfo()
{
	if ( NoAuthCollisionEnableInfo.bEnable )
	{
		// 생명주기 설정
		_SetLifeTime( NoAuthCollisionEnableInfo.LifeTime );
		
		// enable Collision
		SetActorEnableCollision( true );

		return;
	}

	_TearDownDetect();
}
