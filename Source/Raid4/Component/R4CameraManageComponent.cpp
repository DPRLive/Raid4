// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CameraManageComponent.h"
#include "../Data/DataAsset/R4DataAssetPCCamera.h"
#include "../Character/CharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CameraManageComponent)

UR4CameraManageComponent::UR4CameraManageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

/**
 *	컴포넌트 초기화
 */
void UR4CameraManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(ACharacterBase* character = Cast<ACharacterBase>(GetOwner()); CameraInitData != nullptr)
	{
		CameraInitData->LoadDataToCharacter(character);
	}
}

/**
 *  begin play
 */
void UR4CameraManageComponent::BeginPlay()
{
	Super::BeginPlay();
}