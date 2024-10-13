// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include "R4CharacterMovementComponent.generated.h"

/**
 * FSavedMove_Character represents a saved move on the client
 * that has been sent to the server and might need to be played back.
 */
class FR4SavedMove_Character : public FSavedMove_Character
{
	using Super = FSavedMove_Character;
	
public:
	virtual void SetInitialPosition( ACharacter* InCharacter ) override;
};

class FR4NetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	FR4NetworkPredictionData_Client_Character( const UCharacterMovementComponent& ClientMovement )
	: FNetworkPredictionData_Client_Character( ClientMovement )
	{

	}
	
	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * Character가 사용할 R4Character Movement Component.
 * Curve Vector는 Target Loc까지 Duration동안 이동하며, CurveVector의 X,Y,Z를 따라 이동.
 * Curve Vector는 주어진 Duration 동안 각 X,Y,Z의 값의 Curve Y축 값을 추출해 적용하며
 * X,Y,Z값의 Curve Y축 값은 Curve X축(시간당 위치 (0:StartLoc ~ 1:TargetLoc 까지 사용))으로 추출.
 * X,Y,Z값은 시작지점 <-> 목표지점에 선형의 줄을 긋고 시간 비율에 맞는 위치로부터의 Relative Location으로 적용함.
 * TODO : 중간에 Force Move가 끊기는 경우가 있는지?
 */
UCLASS()
class RAID4_API UR4CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UR4CharacterMovementComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	// Movement Update
	virtual void OnMovementUpdated( float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity ) override;

public:
	// Local에서 목표 지점까지 선형으로 Force Movement 설정
	void SetForceMovementByLinear_Local( const FVector& InTargetLoc, float InDuration  );

	// Local에서 CurveVector값을 이용하여 Force Movement 설정
	void SetForceMovementByCurve_Local( const FVector& InTargetLoc, float InDuration, UCurveVector* InCurveVector, bool InIsReverse );

	// Force Move Type Getter
	FORCEINLINE ER4ForceMoveType GetForceMoveType() const { return CachedForceMoveType; }

	// Force Move 정리
	virtual void ClearForceMove();
private:
	// Force Movement 준비
	void _SetupForceMovement( const FVector& InTargetLoc, float InDuration );

	// Force Movement 해제
	void _TearDownForceMovement();

private:
	// Force Move시 사용할 타입
	UPROPERTY( Transient, VisibleInstanceOnly )
	ER4ForceMoveType CachedForceMoveType;

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

	uint8 CachedIsReverseCurve:1;
};
