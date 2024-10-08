// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Tasks/BTTask_BlackboardBase.h>
#include "BTTask_ClearBlackboardValue.generated.h"

/**
 * 지정된 black board 값을 초기화.
 */
UCLASS()
class RAID4_API UBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ClearBlackboardValue();
	
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) override;
};
