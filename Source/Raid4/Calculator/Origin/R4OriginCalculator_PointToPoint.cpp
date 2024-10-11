// Fill out your copyright notice in the Description page of Project Settings.


#include "R4OriginCalculator_PointToPoint.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4OriginCalculator_PointToPoint)

/**
 *  두개의 Calculator를 통해서, A -> B를 바라보는 A 기준 Origin을 Calculate
 *  CDO를 사용!
 *  @param InRequestObj : 계산을 요청한 Object
 *  @param InActor : 계산의 기준이 되는 Actor.
 */
FTransform UR4OriginCalculator_PointToPoint::CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const
{
	if ( !IsValid( AOriginCalculator ) || !IsValid(BOriginCalculator) )
	{
		LOG_WARN( R4Log, TEXT("UR4OriginCalculator_PointToPoint requires valid A and B Origin Calculator.") );
		return FTransform::Identity;
	}

	// A 의 Origin
	FTransform originA = _CalculateOriginByCDO( AOriginCalculator, InRequestObj, InActor );
	FTransform originB = _CalculateOriginByCDO( BOriginCalculator, InRequestObj, InActor );

	// A to B Dir Vector
	FVector dir = originB.GetLocation() - originA.GetLocation();
	FQuat rot = FRotationMatrix::MakeFromX( dir ).ToQuat();

	// set A origin rotation
	originA.SetRotation( rot );

	return originA;
}

/**
 *  CDO를 사용해서 Calculate
 */
FTransform UR4OriginCalculator_PointToPoint::_CalculateOriginByCDO( const TSubclassOf<UObject>& InOriginCalculator, const UObject* InRequestObj, const AActor* InActor ) const
{
	const UObject* cdo = InOriginCalculator->GetDefaultObject( true );
	const IR4OriginCalculatorInterface* originCalculator = Cast<IR4OriginCalculatorInterface>( cdo );
	if( originCalculator == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Origin Calculator is nullptr.") )
		return FTransform::Identity;
	}

	return originCalculator->CalculateOrigin( InRequestObj, InActor );
}
