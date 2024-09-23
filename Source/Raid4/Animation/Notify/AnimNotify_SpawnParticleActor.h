// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_SpawnParticleActor.generated.h"

class AR4ParticleActor;

/**
 * 조금 복잡한 Particle을 확장해서 Spawn할 수 있도록 도와주는 AnimNotify
 */
UCLASS()
class RAID4_API UAnimNotify_SpawnParticleActor : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SpawnParticleActor();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

private:
	// Particle Actor to Spawn
	UPROPERTY( EditAnywhere, Category="AnimNotify" )
	TSubclassOf<AR4ParticleActor> ParticleActorClass;

	// Relative Location
	UPROPERTY( EditAnywhere, Category="AnimNotify" )
	FVector RelativeLocation;

	// Relative Rot
	UPROPERTY( EditAnywhere, Category="AnimNotify" )
	FRotator RelativeRotation;

	// Absolute Scale
	UPROPERTY( EditAnywhere, Category="AnimNotify" )
	FVector Scale;

	// Should attach to the bone/socket
	UPROPERTY( EditAnywhere, Category="AnimNotify" )
	uint32 Attached:1;

	// SocketName to attach to
	UPROPERTY( EditAnywhere, Category = "AnimNotify" )
	FName SocketName;
};
