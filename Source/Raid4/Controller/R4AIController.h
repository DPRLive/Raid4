// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <AIController.h>
#include "R4AIController.generated.h"

/**
 * AI에 사용할 AI Controller
 */
UCLASS()
class RAID4_API AR4AIController : public AAIController
{
	GENERATED_BODY()

public:
	AR4AIController();

protected:
	virtual void OnPossess( APawn* InPawn ) override;

public:
	// AI 구동
	UFUNCTION()
	void RunAI();

	// AI 중지
	UFUNCTION()
	void StopAI();
	
	UFUNCTION()
	void OnDeadAI( AActor* InDeadActor );

private:
	// Black board data
	UPROPERTY( EditDefaultsOnly, Category = AI)
	TObjectPtr<UBlackboardData> BlackboardData;

	// Behavior Tree
	UPROPERTY( EditDefaultsOnly, Category = AI )
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
