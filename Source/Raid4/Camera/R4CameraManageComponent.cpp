// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CameraManageComponent.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CameraManageComponent)

UR4CameraManageComponent::UR4CameraManageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SpringArmLengthTolerance = 0.5f;
	CachedNewSpringArmLength = 0.f;
	CachedSpringArmUpdateSpeed = 0.f;
}

/**
 *  begin play
 */
void UR4CameraManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// 필요할 때 켜서 사용
	SetComponentTickEnabled( false );
}

/**
 *  Tick
 */
void UR4CameraManageComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	bool bNeedUpdate = false;

	bNeedUpdate |= _UpdateSpringArmLength( DeltaTime );

	if ( !bNeedUpdate )
	{
		SetComponentTickEnabled( false );
	}
}

void UR4CameraManageComponent::SetCameraComp( UCameraComponent* InCameraComp )
{
	CachedCameraComp = InCameraComp;
}

void UR4CameraManageComponent::SetSpringArmComp( USpringArmComponent* InSpringArComp )
{
	CachedSpringArmComp = InSpringArComp;
}

/**
 *  Spring Arm Length Update 요청
 *  @param InNewLength : 변경할 값
 *  @param InSpeed : 현재 값 -> 변경할 값 까지의 변경 지연 속도, <= 0일 경우 즉시 이동 
 */
void UR4CameraManageComponent::SetSpringArmLength( float InNewLength, float InSpeed )
{
	if ( !CachedSpringArmComp.IsValid() )
		return;
	
	if ( InSpeed < KINDA_SMALL_NUMBER )
	{
		CachedSpringArmComp->TargetArmLength = InNewLength;
		return;
	}

	CachedNewSpringArmLength = InNewLength;
	CachedSpringArmUpdateSpeed = InSpeed;
	
	SetComponentTickEnabled( true );
}

/**
 *  Spring Arm을 Update
 *  @return : Update가 되었는지 여부, false 시 더이상 Update가 필요 없다는 뜻
 */
bool UR4CameraManageComponent::_UpdateSpringArmLength( float InDeltaTime ) const
{
	if ( !CachedSpringArmComp.IsValid() )
		return false;

	// 차이가 SpringArmLengthTolerance 이하면 중지
	if ( FMath::Abs( CachedNewSpringArmLength - CachedSpringArmComp->TargetArmLength ) <= SpringArmLengthTolerance )
		return false;
	
	float newLength = FMath::FInterpTo( CachedSpringArmComp->TargetArmLength, CachedNewSpringArmLength, InDeltaTime, CachedSpringArmUpdateSpeed );
	CachedSpringArmComp->TargetArmLength = newLength;

	LOG_WARN( LogTemp, TEXT("%f, %f"), newLength, CachedNewSpringArmLength );
	
	return true;
}