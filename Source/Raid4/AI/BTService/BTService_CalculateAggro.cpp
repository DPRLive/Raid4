// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CalculateAggro.h"
#include "../R4CharacterAIInterface.h"
#include "../../Character/R4CharacterBase.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTService_CalculateAggro)

UBTService_CalculateAggro::UBTService_CalculateAggro()
{
	NodeName = TEXT("CalculateAggro");

	// 일단 기본 10초에 한번 계산
	Interval = 10.f;

	// AActor로 키 제한
	BlackboardKey.AddObjectFilter( this, GET_MEMBER_NAME_CHECKED( UBTService_CalculateAggro, BlackboardKey ), AActor::StaticClass() );
}

/**
 *	Tick Node
 */
void UBTService_CalculateAggro::TickNode( UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds )
{
	Super::TickNode( InOwnerComp, InNodeMemory, InDeltaSeconds );

	APawn* controllingPawn = InOwnerComp.GetAIOwner()->GetPawn();
	if ( !IsValid(controllingPawn) )
		return;

	IR4CharacterAIInterface* aiPawn = Cast<IR4CharacterAIInterface>( controllingPawn );
	if( aiPawn == nullptr )
		return;

	UBlackboardComponent* blackboard = InOwnerComp.GetBlackboardComponent();
	if ( !IsValid( blackboard ) )
		return;
		
	AActor* maxAggroTarget = nullptr;
	float maxAggro = 0.f;
	
	// Damage를 입힌 Controller들 중에서 Target을 설정
	for( const auto& [controller, damage] : aiPawn->GetDamagedControllers() )
	{
		if ( !controller.IsValid() )
			continue;

		AR4CharacterBase* newTarget = Cast<AR4CharacterBase>( controller->GetPawn() );
		if( !IsValid( newTarget ) || newTarget->IsDead() )
			continue;

		float newTargetAggro = damage;
		
		// target이 설정 안 된 상태이면, 바로 설정
		if ( !IsValid( maxAggroTarget ) || newTargetAggro > maxAggro )
		{
			maxAggroTarget = newTarget;
			maxAggro = newTargetAggro;
		}
	}

	// 새로 선정된 Target을 BBKey에 저장
	blackboard->SetValue<UBlackboardKeyType_Object>( BlackboardKey.GetSelectedKeyID(), maxAggroTarget );
}