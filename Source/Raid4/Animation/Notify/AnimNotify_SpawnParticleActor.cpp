// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnParticleActor.h"

#include "../../Effect/R4ParticleActor.h"

#include <Components/SkeletalMeshComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_SpawnParticleActor)

UAnimNotify_SpawnParticleActor::UAnimNotify_SpawnParticleActor()
{
	Scale = FVector(1.f);
	Attached = false;
	SocketName = NAME_None;
}

FString UAnimNotify_SpawnParticleActor::GetNotifyName_Implementation() const
{
	if ( IsValid( ParticleActorClass ) )
		return ParticleActorClass->GetName();
	
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_SpawnParticleActor::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( !IsValid( MeshComp ) || !IsValid( ParticleActorClass ))
		return;

	UWorld* world = MeshComp->GetWorld();
	UR4GameInstance* gameInstance = Cast<UR4GameInstance>(world->GetGameInstance());
	
	// Particle Actor를 생성.
	AR4ParticleActor* particleActor = nullptr;
	if(IsValid( gameInstance ))
	{
		particleActor = Cast<AR4ParticleActor>(gameInstance->GetObjectPool()->GetObject( ParticleActorClass ));
	}
	else
		particleActor = Cast<AR4ParticleActor>(world->SpawnActor( ParticleActorClass ));
		
	if ( !IsValid(particleActor) )
	{
		LOG_WARN( R4Effect, TEXT("particleActor is invalid.") );
		return;
	}

	// get origin
	const FTransform& origin = MeshComp->GetSocketTransform( SocketName );
	FTransform spawnOrigin;
	spawnOrigin.SetLocation( origin.TransformPosition( RelativeLocation ) );
	spawnOrigin.SetRotation( origin.TransformRotation( RelativeRotation.Quaternion() ) );
	spawnOrigin.SetScale3D( Scale );

	// Execute
	particleActor->Execute( spawnOrigin );
}
