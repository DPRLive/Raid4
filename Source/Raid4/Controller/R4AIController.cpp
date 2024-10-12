// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AIController.h"
#include "../Damage/R4DamageReceiveInterface.h"

#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AIController)

AR4AIController::AR4AIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AR4AIController::OnPossess( APawn* InPawn )
{
	Super::OnPossess( InPawn );

	RunAI();

	// Character가 Damage를 받아 사망 시 Stop Tree
	if ( IR4DamageReceiveInterface* pawn = Cast<IR4DamageReceiveInterface>(	InPawn ) )
		pawn->OnDead().AddDynamic( this, &AR4AIController::StopAI );
}

/**
 *	AI 구동
 */
void AR4AIController::RunAI()
{
	UBlackboardComponent* blackboardComp = Blackboard.Get();
	if ( UseBlackboard( BlackboardData, blackboardComp ) )
		RunBehaviorTree( BehaviorTree );
}

/**
 *	AI 중지
 */
void AR4AIController::StopAI()
{
	if ( UBehaviorTreeComponent* behaviorTreeComp = Cast< UBehaviorTreeComponent >( BrainComponent ) )
		behaviorTreeComp->StopTree();
}

