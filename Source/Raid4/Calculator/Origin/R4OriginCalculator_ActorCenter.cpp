// Fill out your copyright notice in the Description page of Project Settings.


#include "R4OriginCalculator_ActorCenter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4OriginCalculator_ActorCenter)

/**
 * Actor의 Center를 Origin으로 계산.
 */
FTransform UR4OriginCalculator_ActorCenter::CalculateOrigin( const AActor* InActor ) const
{
	if( !IsValid( InActor ) )
	{
		LOG_WARN( R4Log, TEXT(" InActor is invalid. ") );
		return FTransform::Identity;
	}

	return InActor->GetTransform();
}
