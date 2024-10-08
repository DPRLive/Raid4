// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAISkill.h"
#include "../R4CharacterAIInterface.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Int.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_SelectAISkill)

UBTTask_SelectAISkill::UBTTask_SelectAISkill()
{
	NodeName = TEXT("SelectAISkill");
	
	// int로 키 타입 제한
	SkillIndexBlackboardKey.AddIntFilter( this, GET_MEMBER_NAME_CHECKED( UBTTask_SelectAISkill, SkillIndexBlackboardKey ) );

	// float로 키 타입 제한
	SkillDistBlackboardKey.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UBTTask_SelectAISkill, SkillDistBlackboardKey ) );
}

/**
 * Init
 */
void UBTTask_SelectAISkill::InitializeFromAsset( UBehaviorTree& Asset )
{
	Super::InitializeFromAsset( Asset );
	
	if ( UBlackboardData* blackboard = GetBlackboardAsset() )
	{
		SkillIndexBlackboardKey.ResolveSelectedKey( *blackboard );
		SkillDistBlackboardKey.ResolveSelectedKey( *blackboard );
	}
}

/**
 * Skill 선택.
 */
EBTNodeResult::Type UBTTask_SelectAISkill::ExecuteTask( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory )
{
	if ( Super::ExecuteTask( InOwnerComp, InNodeMemory ) == EBTNodeResult::Failed )
		return EBTNodeResult::Failed;

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid( controllingPawn ) )
		return EBTNodeResult::Failed;

	IR4CharacterAIInterface* aiPawn = Cast< IR4CharacterAIInterface >( controllingPawn );
	if ( aiPawn == nullptr )
		return EBTNodeResult::Failed;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return EBTNodeResult::Failed;

	// 사용 가능한 AI Skill의 Index와 Dist Get
	float skillDist = 0.f;
	int32 skillIdx = aiPawn->GetAvailableMaxDistSkillIndex( skillDist );
	
	// BB에 저장
	blackboard->SetValue<UBlackboardKeyType_Int>( SkillIndexBlackboardKey.GetSelectedKeyID(), skillIdx );
	blackboard->SetValue<UBlackboardKeyType_Float>( SkillDistBlackboardKey.GetSelectedKeyID(), skillDist );

	if ( skillIdx == INDEX_NONE )
		return EBTNodeResult::Failed;
	
	return EBTNodeResult::Succeeded;
}