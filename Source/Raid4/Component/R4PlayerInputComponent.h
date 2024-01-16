// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4PlayerInputComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;
struct FInputActionValue;

/**
 *  입력을 담당하는 Component
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	// 입력을 바인딩 하는 함수
	void _InitializePlayerInput(UInputComponent* InPlayerInputComponent);
	
	// Move Input Func
	void OnInputMoveStarted();
	void OnInputMoveTriggered();
	void OnInputMoveCompleted();

	// Skill Input Func
	void OnInputSkillStarted(const FInputActionValue& InValue, const ESkillIndex InSkillIndex);
	void OnInputSkillTriggered(const FInputActionValue& InValue, const ESkillIndex InSkillIndex);
	void OnInputSkillCompleted(const FInputActionValue& InValue, const ESkillIndex InSkillIndex);
	
	// MappingContext
	UPROPERTY( EditAnywhere, Category = "Data|Mapping", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputMappingContext> InputMapping;
	
	// 이동 입력 액션
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UInputAction> MoveAction;

	// 스킬 입력 액션들
	UPROPERTY( EditAnywhere, Category = "Data|Action", meta = (AllowPrivateAccess = true) )
	TMap<ESkillIndex, TObjectPtr<UInputAction>> SkillActions;
	
	// 짧은 입력 / 긴 입력을 구분하는 경계시간
	UPROPERTY( EditAnywhere, Category = "Data|Setting", meta = (AllowPrivateAccess = true) )
	float ShortTriggerThreshold;

	// 클릭시 스폰되는 FX (어디를 클릭했는지 표시)
	UPROPERTY( EditAnywhere, Category = "Data|Setting", meta=(AllowPrivateAccess = true) )
	TSoftObjectPtr<UNiagaraSystem> FXCursor;
	
	/////////////////////////
	// **** Transient **** //
	/////////////////////////
	
	// 눌린 시간을 측정하는 임시 변수
	UPROPERTY( Transient )
	float TriggerTime;

	// 마지막에 눌린 위치를 기억하는 변수
	UPROPERTY( Transient )
	FVector LastHitLocation;

};
