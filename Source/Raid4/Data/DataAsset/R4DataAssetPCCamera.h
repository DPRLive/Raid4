// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "../../Interface/CharacterLoadableData.h"
#include "R4DataAssetPCCamera.generated.h"

/**
 * PlayerCharacter 가 사용할 초기 Camera Data  
 */
UCLASS()
class RAID4_API UR4DataAssetPCCamera : public UPrimaryDataAsset, public ICharacterLoadableData
{
	GENERATED_BODY()
public:
	UR4DataAssetPCCamera();
	
	virtual void LoadDataToCharacter(ACharacterBase* InCharacter) override;
	
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