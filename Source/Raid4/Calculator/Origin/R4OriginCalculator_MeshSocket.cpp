// Fill out your copyright notice in the Description page of Project Settings.


#include "R4OriginCalculator_MeshSocket.h"

#include <GameFramework/Character.h>
#include <Components/SkeletalMeshComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4OriginCalculator_MeshSocket)

/**
 * Mesh의 Socket을 기준으로 Origin으로 계산.
 */
FTransform UR4OriginCalculator_MeshSocket::CalculateOrigin( const AActor* InActor ) const
{
	const ACharacter* owner = Cast<ACharacter>( InActor );
	if( !IsValid( owner ) )
	{
		LOG_WARN( R4Log, TEXT("UR4OriginCalculator_MeshSocket requires 'ACharacter' based class") );
		return FTransform::Identity;
	}

	const USkeletalMeshComponent* skelMesh = owner->GetMesh();
	if( !IsValid( skelMesh ) )
	{
		LOG_WARN( R4Log, TEXT("USkeletalMeshComponent is invalid.") );
		return FTransform::Identity;
	}

	if ( !skelMesh->DoesSocketExist( SocketName ) )
	{
		LOG_WARN( R4Log, TEXT("SocketName is invalid.") );
		return FTransform::Identity;
	}

	return skelMesh->GetSocketTransform( SocketName );
}
