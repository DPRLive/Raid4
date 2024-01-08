// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillManageComponent.generated.h"

/**
 * 객체가 스킬을 사용할 때, 스킬들을 들고 관리해주는 컴포넌트
 * 객체에 IR4SkillInterface를 오버라이드 후 사용
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4SkillManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillManageComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;
protected:
	// BeginPlay
	virtual void BeginPlay() override;
};
