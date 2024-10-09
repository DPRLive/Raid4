// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ActivateAISkill.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Int.h>
#include <Raid4/AI/R4CharacterAIInterface.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_ActivateAISkill)

UBTTask_ActivateAISkill::UBTTask_ActivateAISkill()
{
	NodeName = TEXT("ActivateAISkill");
	
	// int로 키 타입 제한
	BlackboardKey.AddIntFilter( this, GET_MEMBER_NAME_CHECKED( UBTTask_ActivateAISkill, BlackboardKey ) );
}

EBTNodeResult::Type UBTTask_ActivateAISkill::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	if ( Super::ExecuteTask( InOwnerComp, InNodeMemory ) == EBTNodeResult::Failed )
		return EBTNodeResult::Failed;

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;
		
	IR4CharacterAIInterface* aiPawn = Cast<IR4CharacterAIInterface>( controllingPawn );
	if( aiPawn == nullptr )
		return EBTNodeResult::Failed;
	
	int32 skillIndex = blackboard->GetValue<UBlackboardKeyType_Int>( BlackboardKey.GetSelectedKeyID() );
	FSimpleMulticastDelegate* onEndSkill = aiPawn->ActivateAISkill( skillIndex );

	if ( onEndSkill == nullptr )
		return EBTNodeResult::Failed;

	// this capture, use weak lambda
	onEndSkill->AddWeakLambda( this, [this, &InOwnerComp]()
	{
		FinishLatentTask( InOwnerComp, EBTNodeResult::Succeeded );
	} );
	
	return EBTNodeResult::InProgress;
}
