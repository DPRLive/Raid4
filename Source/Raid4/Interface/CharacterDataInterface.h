// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "CharacterDataInterface.generated.h"

class ACharacterBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 캐릭터가 로드 가능한 데이터를 정의시 사용
 */
class RAID4_API ICharacterDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void LoadDataToCharacter(ACharacterBase* InCharacter) = 0;
};
