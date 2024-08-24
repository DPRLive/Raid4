// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4PlayerInputInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4PlayerInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어의 여러 입력을 처리할 수 있도록 하는 Interface.
 */
class RAID4_API IR4PlayerInputInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 사용될 PlayerController를 return
	virtual APlayerController* GetPlayerController() = 0;

	// PlayerController 에서의 Input Bind 시 InputComp를 받아오기 위한 delegate getter
	DECLARE_MULTICAST_DELEGATE_OneParam( FSetupPlayerInputDelegate, UInputComponent* /* InInputComponent */ )
	virtual FSetupPlayerInputDelegate& OnSetupPlayerInput() = 0;

	// 이동 입력 처리
	virtual void OnInputMoveTriggered(const FVector2D& InDirection) {}

	// 마우스 Look 처리
	virtual void OnInputLookTriggered(const FRotator& InDelta) {}

	// 점프 입력 처리
	virtual void OnInputJumpStarted() {}
	virtual void OnInputJumpCompleted() {}

	// 회피 입력 처리
	virtual void OnInputEvasionStarted() {}
	
	// 스킬 입력 처리
	virtual void OnInputSkillStarted(ESkillIndex InSkillIndex) {}
	virtual void OnInputSkillTriggered(ESkillIndex InSkillIndex) {}
	virtual void OnInputSkillCompleted(ESkillIndex InSkillIndex) {}
};