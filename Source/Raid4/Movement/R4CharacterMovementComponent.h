// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include "R4CharacterMovementComponent.generated.h"

/**
 * Character가 사용할 R4Character Movement Component.
 * Curve Vector는 Target Loc까지 Duration동안 이동하며, CurveVector의 X,Y,Z를 따라 이동.
 * Curve Vector는 주어진 Duration 동안 각 X,Y,Z의 값의 Curve Y축 값을 추출해 적용하며
 * X,Y,Z값의 Curve Y축 값은 Curve X축(시간당 위치 (0:StartLoc ~ 1:TargetLoc 까지 사용))으로 추출.
 * X,Y,Z값은 시작지점 <-> 목표지점에 선형의 줄을 긋고 시간 비율에 맞는 위치로부터의 Relative Location으로 적용함.
 */
UCLASS()
class RAID4_API UR4CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UR4CharacterMovementComponent();

protected:
	// Movement Update
	virtual void OnMovementUpdated( float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity ) override;

public:
	// Local에서 목표 지점까지 선형으로 Force Movement 설정
	void SetForceMovementByLinear_Local( const FVector& InTargetLoc, float InDuration  );

	// Local에서 CurveVector값을 이용하여 Force Movement 설정
	void SetForceMovementByCurve_Local( const FVector& InTargetLoc, float InDuration, UCurveVector* InCurveVector, bool InIsReverse );

private:
	// Force Movement 준비
	void _SetupForceMovement( const FVector& InTargetLoc, float InDuration );

	// Force Movement 해제
	void _TearDownForceMovement();

private:
	// Force Move시 사용할 타입
	UPROPERTY( VisibleInstanceOnly )
	ER4ForceMoveType ForceMoveType;

	// Force Move를 시작한 지점
	FVector CachedForceMoveStartWorldLoc;
	
	// Force Move 목표 지점
	FVector CachedForceMoveTargetWorldLoc;
	
	// Force Move가 진행된 시간 캐싱
	float CachedForceMoveElapsedTime;

	// Force Move 총 기간 캐싱
	float CachedForceMoveDuration;
	
	// For Curve Move//
	// 사용할 Curve Vector.
	TWeakObjectPtr<UCurveVector> CachedCurveVector;

	bool bReverseCurve;
};
