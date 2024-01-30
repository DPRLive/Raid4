// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "../Interface/R4PlayerSkillInputInterface.h"
#include "PlayerCharacter.generated.h"

class UR4PlayerInputComponent;
class UR4CameraManageComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 *  PlayerCharacter의 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API APlayerCharacter : public ACharacterBase, public IR4PlayerSkillInputInterface
{
	GENERATED_BODY()
	
public:
	APlayerCharacter(const FObjectInitializer& InObjectInitializer);

	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent) override;

	// 스킬 입력 처리
	virtual void OnInputSkillStarted(const ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillTriggered(const ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillCompleted(const ESkillIndex InSkillIndex) override;
	
	// 입력 바인딩을 위임
	DECLARE_MULTICAST_DELEGATE_OneParam( FSetupPlayerInputDelegate, UInputComponent* /* InInputComponent */ )
	FSetupPlayerInputDelegate OnSetupPlayerInput;

private:
	// 입력을 담당하는 Input Component
	UPROPERTY( VisibleAnywhere, Category = "Input", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4PlayerInputComponent> PlayerInputComp;

	// Spring Arm
	UPROPERTY( VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true) )
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// 카메라
	UPROPERTY( VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UCameraComponent> CameraComp;
	
	// 카메라를 관리해주는 Component
	UPROPERTY( VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4CameraManageComponent> CameraManageComp;
};
