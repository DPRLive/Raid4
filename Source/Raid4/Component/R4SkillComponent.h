// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillComponent.generated.h"

class UR4SkillBase;

/**
 * 스킬 기능을 부여해주는 SkillComponent
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// BeginPlay
	virtual void BeginPlay() override;

public:
	// 스킬을 추가한다. (서버)
	void Server_AddSkill(const ESkillIndex InSkillIndex, UR4SkillBase* InSkill);

protected:
	// 인스턴스화된 스킬 배열
	UPROPERTY( Replicated, Transient, VisibleAnywhere )
	TArray<TObjectPtr<UR4SkillBase>> InstancedSkills;
};
