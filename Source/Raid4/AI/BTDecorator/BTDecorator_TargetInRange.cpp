// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_TargetInRange.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTDecorator_TargetInRange)

UBTDecorator_TargetInRange::UBTDecorator_TargetInRange()
{
	NodeName = TEXT("TargetInRange");
	
	// AActor로 키 제한
	TargetBlackboardKey.AddObjectFilter( this, GET_MEMBER_NAME_CHECKED( UBTDecorator_TargetInRange, TargetBlackboardKey ), AActor::StaticClass() );
	
	// float로 키 타입 제한
	RangeBlackboardKey.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UBTDecorator_TargetInRange, RangeBlackboardKey ) );
}

void UBTDecorator_TargetInRange::InitializeFromAsset( UBehaviorTree& Asset )
{
	Super::InitializeFromAsset( Asset );

	if ( UBlackboardData* blackboard = GetBlackboardAsset() )
	{
		TargetBlackboardKey.ResolveSelectedKey( *blackboard );
		RangeBlackboardKey.ResolveSelectedKey( *blackboard );
	}
}

bool UBTDecorator_TargetInRange::CalculateRawConditionValue( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory ) const
{
	if ( !Super::CalculateRawConditionValue( InOwnerComp, InNodeMemory ) )
		return false;

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return false;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return false;

	UObject* targetObj = blackboard->GetValue<UBlackboardKeyType_Object>( TargetBlackboardKey.GetSelectedKeyID() );
	AActor* targetActor = Cast<AActor>( targetObj );
	if( !IsValid( targetActor ) )
		return false;
	
	float range = blackboard->GetValue<UBlackboardKeyType_Float>( RangeBlackboardKey.GetSelectedKeyID() );

	return range >= controllingPawn->GetDistanceTo( targetActor );
}
