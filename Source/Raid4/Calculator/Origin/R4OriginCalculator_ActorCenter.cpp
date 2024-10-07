// Fill out your copyright notice in the Description page of Project Settings.


#include "R4OriginCalculator_ActorCenter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4OriginCalculator_ActorCenter)

/**
 *  Actor의 Center를 Origin으로 계산.
 *  CDO를 사용!
 *  @param InRequestObj : 계산을 요청한 Object
 *  @param InActor : 계산의 기준이 되는 Actor.
 */
FTransform UR4OriginCalculator_ActorCenter::CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const
{
	if( !IsValid( InActor ) )
	{
		LOG_WARN( R4Log, TEXT(" InActor is invalid. ") );
		return FTransform::Identity;
	}

	return InActor->GetTransform();
}
