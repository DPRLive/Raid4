﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4CharacterBase.h"
#include "../../AI/R4CharacterAIInterface.h"
#include "R4NonPlayerCharacter.generated.h"

/**
 *  NonPlayerCharacter ( Monster 등 )
 */
UCLASS()
class RAID4_API AR4NonPlayerCharacter : public AR4CharacterBase, public IR4CharacterAIInterface
{
	GENERATED_BODY()

public:
	AR4NonPlayerCharacter( const FObjectInitializer& InObjectInitializer );

protected:
	virtual void BeginPlay() override;

public:
	// ~ Begin IR4CharacterAIInterface
	virtual const TMap<TWeakObjectPtr<const AController>, float>& GetDamagedControllers() const override { return DamagedControllerList; }
	// ~ End IR4CharacterAIInterface

	UFUNCTION( )
	void OnAICharacterDamaged( const AActor* InInstigator, float InDamage );
private:
	// Damage를 입힌 Actor들의 List를 Caching
	TMap<TWeakObjectPtr<const AController>, float> DamagedControllerList;
};
