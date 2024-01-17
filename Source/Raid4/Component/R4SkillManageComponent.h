// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillManageComponent.generated.h"

/**
 * 객체의 스킬들을 관리해주는 컴포넌트.
 * 사용중인 스킬 상태, 스킬 사용 가능 여부 판단 등
 * 객체에 IR4SkillManageInterface를 오버라이드 후 사용 -> 여기서 버프같은거 체크 시킬 생각
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
