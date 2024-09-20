// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4CharacterBase.h"
#include "../../Input/R4PlayerInputInterface.h"
#include "R4PlayerCharacter.generated.h"

class UR4DataAsset_PCCommonData;
class UR4PlayerInputComponent;
class UR4CameraManageComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 *  PlayerCharacter의 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API AR4PlayerCharacter : public AR4CharacterBase, public IR4PlayerInputInterface
{
	GENERATED_BODY()
	
public:
	AR4PlayerCharacter(const FObjectInitializer& InObjectInitializer);

	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent) override;

	// ~ Begin IR4PlayerInputInterface
	virtual APlayerController* GetPlayerController() override;
	FORCEINLINE virtual FSetupPlayerInputDelegate& OnSetupPlayerInput() override { return OnSetupPlayerInputDelegate; }

	virtual void OnInputMoveTriggered(const FVector2D& InDirection) override;

	virtual void OnInputLookTriggered(const FRotator& InDelta) override;

	virtual void OnInputJumpStarted() override;
	virtual void OnInputJumpCompleted() override;

	virtual void OnInputSkillStarted( EPlayerSkillIndex InSkillIndex ) override;
	virtual void OnInputSkillTriggered( EPlayerSkillIndex InSkillIndex ) override;
	virtual void OnInputSkillCompleted( EPlayerSkillIndex InSkillIndex ) override;
	// ~ End IR4PlayerInputInterface
	
private:
	// PlayerCharacter들의 공통된 데이터를 초기화한다.  
	void _InitPlayerCharacterCommonData();

private:
	// 입력 바인딩을 위임
	FSetupPlayerInputDelegate OnSetupPlayerInputDelegate;
	
	// PlayerCharacter 들이 사용할 공통 데이터들
	UPROPERTY( EditAnywhere, Category = "Data", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4DataAsset_PCCommonData> PlayerCharacterCommonData;
	
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
