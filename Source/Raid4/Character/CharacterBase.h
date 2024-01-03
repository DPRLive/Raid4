// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;
};
