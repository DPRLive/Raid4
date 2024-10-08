// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/Services/BTService_BlackboardBase.h>
#include "BTService_CalculateAggro.generated.h"

/**
 * '공격 받은 데미지'를 기반으로 어그로를 계산해주는 Service Node.
 */
UCLASS()
class RAID4_API UBTService_CalculateAggro : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CalculateAggro();

protected:
	virtual void TickNode( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds ) override;
};
