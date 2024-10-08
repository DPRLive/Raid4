// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "../R4CharacterAIInterface.h"

#include <AIController.h>
#include <NavigationSystem.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_FindPatrolPos)

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
	
	// FVector로 Key 제한
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED( UBTTask_FindPatrolPos, BlackboardKey ) );
}

/**
 * Max Patrol Radius 내에서 Patrol할 위치를 선택.
 */
EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	EBTNodeResult::Type Result = Super::ExecuteTask(InOwnerComp, InNodeMemory);

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return EBTNodeResult::Failed;

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem( controllingPawn->GetWorld() );
	if ( !IsValid( navSystem ) )
		return EBTNodeResult::Failed;
	
	IR4CharacterAIInterface* aiPawn = Cast< IR4CharacterAIInterface >( controllingPawn );
	if ( aiPawn == nullptr )
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;
	
	FVector origin = controllingPawn->GetActorLocation();
	float patrolRadius = aiPawn->GetMaxPatrolRadius();
	FNavLocation nextPatrolPos;

	if ( navSystem->GetRandomPointInNavigableRadius( origin, patrolRadius, nextPatrolPos ) )
	{
		blackboard->SetValue<UBlackboardKeyType_Vector>( BlackboardKey.GetSelectedKeyID(), nextPatrolPos.Location );
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
