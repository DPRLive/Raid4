// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterMovementComponent.h"

#include <Curves/CurveVector.h>
#include <GameFramework/Character.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterMovementComponent)

void FR4SavedMove_Character::SetInitialPosition( ACharacter* InCharacter )
{
	Super::SetInitialPosition( InCharacter );

	// Force Move 시 Combine 하지 않음
	if(UR4CharacterMovementComponent* moveComp = InCharacter->GetCharacterMovement<UR4CharacterMovementComponent>())
	{
		if(moveComp->GetForceMoveType() != ER4ForceMoveType::None)
			bForceNoCombine = true;
	}
}


FSavedMovePtr FR4NetworkPredictionData_Client_Character::AllocateNewMove()
{
	// custom 정보로 변경
	return MakeShared<FR4SavedMove_Character>();
}

UR4CharacterMovementComponent::UR4CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ForceMoveType = ER4ForceMoveType::None;
	CachedForceMoveStartWorldLoc = FVector::ZeroVector;
	CachedForceMoveElapsedTime = 0.f;
	CachedForceMoveDuration = 0.f;
	CachedForceMoveTargetWorldLoc = FVector::ZeroVector;
	CachedCurveVector = nullptr;
	CachedIsReverseCurve = false;
}

class FNetworkPredictionData_Client* UR4CharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UR4CharacterMovementComponent* mutableThis = const_cast<UR4CharacterMovementComponent*>(this);
		mutableThis->ClientPredictionData = new FR4NetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}

/**
 *	Local에서 목표 지점까지 선형으로 Force Movement 설정
 *	목표 지점까지 Duration동안 선형으로 이동.
 *	@param InTargetLoc : 목표 지점
 *	@param InDuration : 걸리는 시간
 */
void UR4CharacterMovementComponent::SetForceMovementByLinear_Local( const FVector& InTargetLoc, float InDuration )
{
	ForceMoveType = ER4ForceMoveType::Linear;
	_SetupForceMovement( InTargetLoc, InDuration );
}

/**
 *	Local에서 CurveVector값을 이용하여 Force Movement 설정
 *	Target Loc까지 Duration동안 이동하며, CurveVector의 X,Y,Z를 따라 이동.
 *	Curve Vector는 주어진 Duration 동안 각 X,Y,Z의 값의 Curve Y축 값을 추출해 적용하며
 *	X,Y,Z값의 Curve Y축 값은 Curve X축(시간당 위치 (0:StartLoc ~ 1:TargetLoc 까지 사용))으로 추출.
 *	X,Y,Z값은 시작지점 <-> 목표지점에 선형의 줄을 긋고 시간 비율에 맞는 위치로부터의 Relative Location으로 적용함.
 *	@param InTargetLoc : 목표 지점
 *	@param InDuration : 걸리는 시간
 *	@param InCurveVector : X,Y,Z의 Delta 값을 담은 Curve Vector
 *	@param InIsReverse : Curve를 반대로 적용할 것인지?
 */
void UR4CharacterMovementComponent::SetForceMovementByCurve_Local( const FVector& InTargetLoc, float InDuration, UCurveVector* InCurveVector, bool InIsReverse )
{
	if ( !IsValid( InCurveVector ) )
	{
		LOG_WARN( R4Data, TEXT("In Curve Asset is invalid.") );
		return;
	}

	ForceMoveType = ER4ForceMoveType::CurveVector;
	_SetupForceMovement( InTargetLoc, InDuration );
	
	CachedCurveVector = InCurveVector;
	CachedIsReverseCurve = InIsReverse;
}

/**
 *	Force Movement 준비
 */
void UR4CharacterMovementComponent::_SetupForceMovement( const FVector& InTargetLoc, float InDuration )
{
	SetMovementMode( MOVE_Custom );
	CachedForceMoveStartWorldLoc = UpdatedComponent->GetComponentLocation();
	CachedForceMoveTargetWorldLoc = InTargetLoc;
	CachedForceMoveElapsedTime = 0.f;
	CachedForceMoveDuration = InDuration;
	bIgnoreClientMovementErrorChecksAndCorrection = true;
}

/**
 *	Force Movement 해제
 */
void UR4CharacterMovementComponent::_TearDownForceMovement()
{
	ForceMoveType = ER4ForceMoveType::None;
	SetDefaultMovementMode();
	bIgnoreClientMovementErrorChecksAndCorrection = false;
}

/**
 *	Movement Update
 */
void UR4CharacterMovementComponent::OnMovementUpdated( float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity )
{
	Super::OnMovementUpdated( DeltaSeconds, OldLocation, OldVelocity );

	if ( ForceMoveType == ER4ForceMoveType::None )
		return;
	
	// Force Movement가 설정되어 있다면 진행
	CachedForceMoveElapsedTime += DeltaSeconds;

	// 현재 시간에 따른 비율
	float ratio = CachedForceMoveElapsedTime / CachedForceMoveDuration;
	
	// 선형 보간한 다음 위치 계산
	FVector nextLoc = FMath::Lerp( CachedForceMoveStartWorldLoc, CachedForceMoveTargetWorldLoc, ratio );
	
	// 목표 지점까지 선형이동 시
	// Curve vector 이용 시
	if ( ForceMoveType == ER4ForceMoveType::CurveVector )
	{
		if ( !CachedCurveVector.IsValid() )
		{
			LOG_WARN( R4Data, TEXT("CachedCurveVector is invalid.") );
			_TearDownForceMovement();
			return;
		}

		FVector curveValue = CachedIsReverseCurve ?
			CachedCurveVector->GetVectorValue( 1.f - ratio ) : CachedCurveVector->GetVectorValue( ratio );

		// Curve 값을 Relative로 사용
		nextLoc = nextLoc + curveValue;
	}
	
	FVector delta = nextLoc - UpdatedComponent->GetComponentLocation();
	UpdatedComponent->MoveComponent(delta, UpdatedComponent->GetComponentQuat(), true);
	
	// 시간이 모두 지났으면 종료
	if ( CachedForceMoveElapsedTime - CachedForceMoveDuration > -KINDA_SMALL_NUMBER )
	{
		_TearDownForceMovement();
	}
}

