// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4PlayerInputComponent.generated.h"

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
	void _InitializePlayerInput(UInputComponent* PlayerInputComponent);
};
