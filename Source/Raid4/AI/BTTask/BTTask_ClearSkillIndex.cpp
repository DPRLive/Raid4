// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearSkillIndex.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Int.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_ClearSkillIndex)

UBTTask_ClearSkillIndex::UBTTask_ClearSkillIndex()
{
	NodeName = TEXT("ClearSkillIndex");

	// int로 타입 제한
	BlackboardKey.AddIntFilter( this, GET_MEMBER_NAME_CHECKED( UBTTask_ClearSkillIndex, BlackboardKey ) );
}

EBTNodeResult::Type UBTTask_ClearSkillIndex::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	if ( Super::ExecuteTask( InOwnerComp, InNodeMemory ) == EBTNodeResult::Failed )
		return EBTNodeResult::Failed;

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;

	blackboard->SetValue<UBlackboardKeyType_Int>( BlackboardKey.GetSelectedKeyID(), INDEX_NONE );
	return EBTNodeResult::Succeeded;
}
