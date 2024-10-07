// Fill out your copyright notice in the Description page of Project Settings.


#include "R4OriginCalculator_FromRequestObj.h"

#include "R4OriginProviderInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4OriginCalculator_FromRequestObj)

/**
 * Request Obj로부터 IR4OriginProviderInterface를 통해 제공 받음.
 * Request Obj가 IR4OriginProviderInterface를 통해 Origin을 제공할 수 있어야함!
 */
FTransform UR4OriginCalculator_FromRequestObj::CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const
{
	// Request Obj로부터 IR4OriginProviderInterface를 통해 제공 받음
	const IR4OriginProviderInterface* requestObj = Cast<IR4OriginProviderInterface>( InRequestObj );
	if( requestObj == nullptr )
	{
		LOG_WARN( R4Log, TEXT("UR4OriginCalculator_FromRequestObj needs RequestObj that inherits IR4OriginProviderInterface.") );
		return FTransform::Identity;
	}

	return requestObj->GetOrigin( InRequestObj, InActor );
}
