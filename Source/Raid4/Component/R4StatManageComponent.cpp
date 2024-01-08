// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatManageComponent)

UR4StatManageComponent::UR4StatManageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

}

/**
 *	컴포넌트 초기화
 */
void UR4StatManageComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

/**
 *  begin play
 */
void UR4StatManageComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

