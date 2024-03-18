// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatComponent.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatComponent)

UR4StatComponent::UR4StatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

/**
 *	컴포넌트 초기화
 */
void UR4StatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);
}

/**
 *  begin play
 */
void UR4StatComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Replicate 설정
 */
void UR4StatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

