// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "PlayerCharacter.generated.h"

class UR4PlayerInputComponent;

/**
 *  PlayerCharacter의 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 입력 바인딩을 위임
	DECLARE_MULTICAST_DELEGATE_OneParam(FSetupPlayerInputDelegate, UInputComponent*)
	FSetupPlayerInputDelegate OnSetupPlayerInput;

private:
	// 입력을 담당하는 Input Component
	UPROPERTY( VisibleAnywhere, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UR4PlayerInputComponent> PlayerInputComp;
};
