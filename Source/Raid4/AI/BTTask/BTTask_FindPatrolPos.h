// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Tasks/BTTask_BlackboardBase.h>
#include "BTTask_FindPatrolPos.generated.h"

/**
 * Random한 Patrol Pos를 찾아서 이동.
 */
UCLASS()
class RAID4_API UBTTask_FindPatrolPos : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolPos();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) override;
};
