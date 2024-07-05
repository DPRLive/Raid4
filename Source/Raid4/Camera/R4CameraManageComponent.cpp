// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CameraManageComponent.h"
#include "../Character/R4CharacterBase.h"

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
	
}

/**
 *  begin play
 */
void UR4CameraManageComponent::BeginPlay()
{
	Super::BeginPlay();
}