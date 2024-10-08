// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Tasks/BTTask_BlackboardBase.h>
#include "BTTask_TurnToTarget.generated.h"

/**
 * target 방향으로 회전
 */
UCLASS()
class RAID4_API UBTTask_TurnToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_TurnToTarget();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) override;
};
