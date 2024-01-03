// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterBase)

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

/**
 *  begin play
 */
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}