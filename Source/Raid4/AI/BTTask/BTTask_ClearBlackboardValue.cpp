// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBlackboardValue.h"

#include <BehaviorTree/BlackboardComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_ClearBlackboardValue)

UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue()
{
	NodeName = TEXT("ClearBlackboardValue");
}

EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	if ( Super::ExecuteTask( InOwnerComp, InNodeMemory ) == EBTNodeResult::Failed )
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;

	// clear value
	blackboard->ClearValue( BlackboardKey.GetSelectedKeyID() );
	return EBTNodeResult::Succeeded;
}
