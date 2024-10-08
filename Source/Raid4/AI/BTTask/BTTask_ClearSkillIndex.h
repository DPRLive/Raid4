// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Tasks/BTTask_BlackboardBase.h>
#include "BTTask_ClearSkillIndex.generated.h"

/**
 * Skill Index를 INDEX_NONE ( -1 ) 로 Clear
 */
UCLASS()
class RAID4_API UBTTask_ClearSkillIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ClearSkillIndex();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) override;
};
