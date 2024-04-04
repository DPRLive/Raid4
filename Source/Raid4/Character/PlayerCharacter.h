// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "../Interface/R4PlayerSkillInputable.h"
#include "../Interface/R4MouseMovable.h"
#include "../Interface/R4PlayerInputCompInterface.h"
#include "PlayerCharacter.generated.h"

class UR4DAPCCommonData;
class UR4PlayerInputComponent;
class UR4CameraManageComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 *  PlayerCharacter의 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API APlayerCharacter : public ACharacterBase, public IR4PlayerInputCompInterface, public IR4MouseMovable, public IR4PlayerSkillInputable
{
	GENERATED_BODY()
	
public:
	APlayerCharacter(const FObjectInitializer& InObjectInitializer);

	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent) override;

	// ~ Begin IR4PlayerInputCompInterface
	virtual APlayerController* GetPlayerController() override;
	FORCEINLINE virtual FSetupPlayerInputDelegate& GetOnSetupPlayerInput() override { return OnSetupPlayerInput; }
	// ~ End IR4PlayerInputCompInterface

	// ~ Begin IR4MouseMovable
	virtual void StopMove() override;
	virtual void AddMovement(const FVector& InWorldDir) override;
	virtual void MoveToLocation(const FVector& InLoc) override;
	// ~ End IR4MouseMovable
	
	// ~ begin IR4PlayerSkillInputable (스킬 입력 처리, SkillComp 로 처리를 위임) 
	virtual void OnInputSkillStarted(ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillTriggered(ESkillIndex InSkillIndex) override;
	virtual void OnInputSkillCompleted(ESkillIndex InSkillIndex) override;
	// ~ end IR4PlayerSkillInputable
	
private:
	// PlayerCharacter들의 공통된 데이터를 초기화한다.  
	void _InitPlayerCharacterCommonData();

private:
	// 입력 바인딩을 위임
	FSetupPlayerInputDelegate OnSetupPlayerInput;
	
	// PlayerCharacter 들이 사용할 공통 데이터들
	UPROPERTY( EditAnywhere, Category = "Data", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4DAPCCommonData> PlayerCharacterCommonData;
	
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
