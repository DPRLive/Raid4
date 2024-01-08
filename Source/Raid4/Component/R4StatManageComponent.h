// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4StatManageComponent.generated.h"

/**
 * 객체에 스탯을 부여할 때, 그 스탯을 관리해주는 컴포넌트
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4StatManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4StatManageComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;
	
protected:
	// BeginPlay
	virtual void BeginPlay() override;

	
};
