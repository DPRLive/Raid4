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
	CachedSpringArmResizeSpeed = 0.f;
}

/**
 *  begin play
 */
void UR4CameraManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Camera Comp, Spring Comp
	if ( IsValid( GetOwner() ) )
	{
		SetCameraComp( GetOwner()->FindComponentByClass<UCameraComponent>() );
		SetSpringArmComp( GetOwner()->FindComponentByClass<USpringArmComponent>() );
	}
	
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
		SetComponentTickEnabled( false );
}

void UR4CameraManageComponent::SetCameraComp( UCameraComponent* InCameraComp )
{
	CachedCameraComp = InCameraComp;
}

void UR4CameraManageComponent::SetSpringArmComp( USpringArmComponent* InSpringArmComp )
{
	CachedSpringArmComp = InSpringArmComp;

	if ( CachedSpringArmComp.IsValid() )
		CachedNewSpringArmLength = CachedSpringArmComp->TargetArmLength;
}

/**
 *  Spring Arm의 길이를 조정 SpringArmResizeSpeed <= 0일 경우 즉시 이동 
 *  @param InDeltaLength : 변화시킬 값
 */
void UR4CameraManageComponent::AddSpringArmLength( float InDeltaLength )
{
	if ( !CachedSpringArmComp.IsValid() )
		return;
	
	CachedNewSpringArmLength += InDeltaLength;
	SetComponentTickEnabled( true );
}

/**
 *  Spring Arm을 Update, SpringArmResizeSpeed <= 0일 경우 즉시 이동
 *  @return : Update가 되었는지 여부, false 시 더이상 Update가 필요 없다는 뜻
 */
bool UR4CameraManageComponent::_UpdateSpringArmLength( float InDeltaTime ) const
{
	if ( !CachedSpringArmComp.IsValid() )
		return false;

	float newLength = 0.f;
	
	// SpringArmResizeSpeed <= 0일 경우 즉시 이동
	if ( CachedSpringArmResizeSpeed < KINDA_SMALL_NUMBER )
		newLength = CachedNewSpringArmLength;
	else
		newLength = FMath::FInterpTo( CachedSpringArmComp->TargetArmLength, CachedNewSpringArmLength, InDeltaTime, CachedSpringArmResizeSpeed );

	CachedSpringArmComp->TargetArmLength = newLength;

	// 차이가 SpringArmLengthTolerance 이하면 중지
	if ( FMath::Abs( CachedNewSpringArmLength - CachedSpringArmComp->TargetArmLength ) <= SpringArmLengthTolerance )
		return false;
	
	return true;
}