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
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
public:
	// ~ Begin IR4CharacterAIInterface
	virtual const TMap<TWeakObjectPtr<const AController>, float>& GetDamagedControllers() const override { return CachedDamagedControllers; }
	virtual float GetMaxPatrolRadius() const override { return MaxPatrolRadius; }
	virtual FSimpleMulticastDelegate* ActivateAISkill( uint8 InSkillIndex ) override;
	virtual int32 GetAvailableMaxDistSkillIndex( float& OutDist ) const override;
	virtual float GetAIRotationSpeed() const override { return AIRotationSpeed; } 
	// ~ End IR4CharacterAIInterface

	// ~ Begin IR4DTDataPushable (Character의 데이터를 초기화한다. ( By DT_Character))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable
	
	// AI 캐릭터 데미지 수신
	UFUNCTION( )
	void OnAICharacterDamaged( const AActor* InInstigator, float InDamage );
	
private:
	// 정찰 시 최대 이동 범위.
	UPROPERTY( EditAnywhere, Category = "AI" )
	float MaxPatrolRadius;

	// AI Controller에 의해 회전 시 회전 속도.
	UPROPERTY( EditAnywhere, Category = "AI" )
	float AIRotationSpeed;
	
	// Damage를 입힌 Actor들의 List를 Caching
	TMap<TWeakObjectPtr<const AController>, float> CachedDamagedControllers;
};
