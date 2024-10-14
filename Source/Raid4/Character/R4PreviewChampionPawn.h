// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/R4DTDataPushInterface.h"

#include <GameFramework/Pawn.h>

#include "R4PreviewChampionPawn.generated.h"

class UAnimInstance;
class UAnimMontage;

/**
 * Champion selection에서 캐릭터 미리보기를 제공하는 Actor
 */
UCLASS()
class RAID4_API AR4PreviewChampionPawn : public APawn, public IR4DTDataPushInterface
{
	GENERATED_BODY()

public:
	AR4PreviewChampionPawn();

	virtual void BeginPlay() override;
	
	virtual void PossessedBy( AController* InNewController ) override;

	virtual void OnRep_PlayerState() override;

public:
	// ~ Begin IR4DTDataPushable
	virtual void PushDTData( FPriKey InPk ) override;
	virtual void ClearDTData() override;
	// ~ End IR4DTDataPushable

private:
	// Mesh Load 성공 시 호출
	UFUNCTION()
	void _MeshLoadComplete();
	
private:
	// Root
	UPROPERTY( VisibleAnywhere )
	TObjectPtr<USceneComponent> RootComp;
	
	// Mesh 미리보기용 Skel Mesh Comp
	UPROPERTY( VisibleAnywhere )
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;

	// Anim Instance
	UPROPERTY( Transient, VisibleInstanceOnly )
	TSubclassOf<UAnimInstance> CachedAnimInstance;
	
	// Start Anim
	UPROPERTY( Transient, VisibleInstanceOnly )
	TObjectPtr<UAnimMontage> CachedStartAnimMontage;
	
	// Mesh Async load handle
	TSharedPtr<FStreamableHandle> CachedMeshHandle;
};
