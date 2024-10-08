// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "../../AI/R4CharacterAIInterface.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_TurnToTarget)

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");

	// AActor로 제한
	BlackboardKey.AddObjectFilter( this, GET_MEMBER_NAME_CHECKED( UBTTask_TurnToTarget, BlackboardKey ), AActor::StaticClass() );
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	if ( Super::ExecuteTask( InOwnerComp, InNodeMemory ) == EBTNodeResult::Failed )
		return EBTNodeResult::Failed;

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return EBTNodeResult::Failed;

	IR4CharacterAIInterface* aiPawn = Cast<IR4CharacterAIInterface>( controllingPawn );
	if ( aiPawn == nullptr )
		return EBTNodeResult::Failed;
	
	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;

	UObject* targetObj = blackboard->GetValue<UBlackboardKeyType_Object>( BlackboardKey.GetSelectedKeyID() );
	AActor* targetActor = Cast<AActor>( targetObj );
	if( !IsValid( targetActor ) || !IsValid( GetWorld() ) )
		return EBTNodeResult::Failed;

	float turnSpeed = aiPawn->GetAIRotationSpeed();
	FVector dir = targetActor->GetActorLocation() - controllingPawn->GetActorLocation();
	dir.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX( dir ).Rotator();
	controllingPawn->SetActorRotation( FMath::RInterpTo( controllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), turnSpeed ) );

	return EBTNodeResult::Succeeded;
}
