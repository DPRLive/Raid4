// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <UObject/WeakInterfacePtr.h>

#include "R4PlayerInputComponent.generated.h"

class IR4PlayerInputInterface;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 *  입력을 담당하는 Component
 */
UCLASS( ClassGroup=(Input), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4PlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4PlayerInputComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;

protected:
	virtual void BeginPlay() override;

private:
	// Input Binding
	void _InitializePlayerInput( UInputComponent* InPlayerInputComponent );

	// Movement Input
	void _OnInputMoveTriggered( const FInputActionValue& Value );

	// Look Input
	void _OnInputLookTriggered( const FInputActionValue& Value );

	// Jump Input
	void _OnInputJumpStarted( const FInputActionValue& Value );
	void _OnInputJumpCompleted( const FInputActionValue& Value );
	
	// Skill Input
	void _OnInputSkillStarted( const FInputActionValue& InValue, EPlayerSkillIndex InSkillIndex );
	void _OnInputSkillTriggered( const FInputActionValue& InValue, EPlayerSkillIndex InSkillIndex );
	void _OnInputSkillCompleted( const FInputActionValue& InValue, EPlayerSkillIndex InSkillIndex );

private:
	// MappingContext
	UPROPERTY( EditAnywhere, Category = "Data|Mapping", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputMappingContext> InputMapping;
	
	// 이동 입력 액션
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputAction> MoveAction;

	// Look 입력 액션
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputAction> LookAction;

	// 점프 입력 액션
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputAction> JumpAction;
	
	// Player 스킬 입력 액션들
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TMap<EPlayerSkillIndex, TObjectPtr<UInputAction>> SkillActions;

	// Cached Owner
	TWeakInterfacePtr<IR4PlayerInputInterface> Owner;
};
