// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillManageComponent)

UR4SkillManageComponent::UR4SkillManageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

/**
 *	컴포넌트 초기화
 */
void UR4SkillManageComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

/**
 *  begin play
 */
void UR4SkillManageComponent::BeginPlay()
{
	Super::BeginPlay();
}
