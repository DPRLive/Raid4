// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerInputComponent.h"
#include "../Character/PlayerCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerInputComponent)

UR4PlayerInputComponent::UR4PlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

/**
 *	컴포넌트 초기화
 */
void UR4PlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Owner를 등록하고, Input Init을 대기한다.
	if(APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner()))
	{
		player->OnSetupPlayerInput.AddUObject(this, &UR4PlayerInputComponent::_InitializePlayerInput);
	}
}

/**
 *  begin play
 */
void UR4PlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  입력을 바인딩 하는 함수
 */
void UR4PlayerInputComponent::_InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	
}
