// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "R4DataAsset_PCCommonData.generated.h"

/**
 * 모든 PlayerCharacter가 사용할 초기 Data  
 */
UCLASS()
class RAID4_API UR4DataAsset_PCCommonData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UR4DataAsset_PCCommonData() :
	bUseControllerRotationYaw(false),
	bOrientRotationToMovement(false),
	bUseControllerDesiredRotation(false),
	RotationRate(FRotator()),
	TargetArmLength(0.f),
	RelativeLocation(FVector()),
	RelativeRotation(FRotator()),
	bUsePawnControlRotation(false),
	bInheritPitch(false),
	bInheritYaw(false),
	bInheritRoll(false),
	bDoCollisionTest(false)
	{ }
	
	UPROPERTY( EditAnywhere, Category = "Pawn" )
	uint8 bUseControllerRotationYaw : 1;

	UPROPERTY( EditAnywhere, Category = "CharacterMovement" )
	uint8 bOrientRotationToMovement : 1;

	UPROPERTY( EditAnywhere, Category = "CharacterMovement" )
	uint8 bUseControllerDesiredRotation : 1;

	UPROPERTY( EditAnywhere, Category = "CharacterMovement" )
	FRotator RotationRate;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	float TargetArmLength;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	FVector RelativeLocation;
	
	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	FRotator RelativeRotation;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	uint8 bUsePawnControlRotation : 1;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	uint8 bInheritPitch : 1;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	uint8 bInheritYaw : 1;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	uint8 bInheritRoll : 1;

	UPROPERTY( EditAnywhere, Category = "SpringArm" )
	uint8 bDoCollisionTest : 1;
};