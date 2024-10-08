// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BTTaskNode.h>
#include "BTTask_SelectAISkill.generated.h"

/**
 * 사용할 Skill을 설정.
 */
UCLASS()
class RAID4_API UBTTask_SelectAISkill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectAISkill();

	// Init
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) override;

protected:
	// 선택한 skillIndex BBKey
	UPROPERTY( EditAnywhere, Category=Blackboard )
	FBlackboardKeySelector SkillIndexBlackboardKey;

	// 사용 가능 범위 BBKey
	UPROPERTY( EditAnywhere, Category=Blackboard )
	FBlackboardKeySelector SkillDistBlackboardKey;
};
