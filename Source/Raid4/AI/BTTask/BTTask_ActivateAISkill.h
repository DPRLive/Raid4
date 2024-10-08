// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Tasks/BTTask_BlackboardBase.h>
#include "BTTask_ActivateAISkill.generated.h"

/**
 * AI Skill을 사용.
 */
UCLASS()
class RAID4_API UBTTask_ActivateAISkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAISkill();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) override;
};
