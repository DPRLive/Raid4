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
	
protected:
	virtual void BeginPlay() override;

public:
	// AI 구동
	void RunAI();

	// AI 중지
	void StopAI();

private:
	// Black board data
	UPROPERTY( EditDefaultsOnly, Category = AI)
	TObjectPtr<UBlackboardData> BlackboardData;

	// Behavior Tree
	UPROPERTY( EditDefaultsOnly, Category = AI )
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
