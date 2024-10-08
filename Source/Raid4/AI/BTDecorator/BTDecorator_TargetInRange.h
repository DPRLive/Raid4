// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BTDecorator.h>
#include "BTDecorator_TargetInRange.generated.h"

/**
 * Target이 범위 내에 있는지 확인하는 Decorator
 */
UCLASS()
class RAID4_API UBTDecorator_TargetInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_TargetInRange();
	
	// Init
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) const override;

protected:
	// Target BBKey
	UPROPERTY( EditAnywhere, Category=Blackboard )
	FBlackboardKeySelector TargetBlackboardKey;

	// 범위 BBKey
	UPROPERTY( EditAnywhere, Category=Blackboard )
	FBlackboardKeySelector RangeBlackboardKey;
};
