// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OverlapCheck.h"
#include "../../Util/UtilOverlap.h"
#include "../../Controller/R4PlayerController.h"
#include "../../Detect/DetectResult.h"

#include <Components/SkeletalMeshComponent.h>
#include <GameFramework/Pawn.h>
#include <Engine/OverlapResult.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_OverlapCheck)

UAnimNotify_OverlapCheck::UAnimNotify_OverlapCheck()
{
	Shape = EOverlapShape::Box;
	bDrawDebug = true;
	DebugTime = 0.f;
	DebugColor = FColor::Green;
}

void UAnimNotify_OverlapCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 클라에서 Notify 시 Server로는 가지 않지만 서버에서도 클라로 히트체크를 굳이 보내지 않도록 함
	const APawn* owner = Cast<APawn>(MeshComp->GetOwner());
	if(!IsValid(owner))
		return;

	const AController* controller = owner->GetController();
	if(!IsValid(controller))
		return;
	
	if (const UWorld* world = MeshComp->GetWorld(); IsValid(world) && controller->IsLocalController())
	{
		// Relative Location을 더한 위치, 회전을 구함
		// TODO : UI용 마우스 위치 버전을 만들어야해! 아하하! 마우스로 회전 기능도 만들어야해!
		const FTransform& meshTrans = MeshComp->GetComponentTransform();
		const FVector center = meshTrans.TransformPosition(RelativeLoc);
		const FQuat quat = meshTrans.GetRotation() * RelativeRot.Quaternion();
		
		FCollisionQueryParams params;
		params.AddIgnoredActor(owner); // 나는 무시
		TArray<FOverlapResult> overlapResults;

		FCollisionResponseParams responseParams(BodyInstance.GetResponseToChannels());
		
		//BodyInstance.
		switch (Shape) // 모양에 맞춰 overlap 체크
		{
		case EOverlapShape::Box:
			UtilOverlap::BoxOverlapByChannel(overlapResults, world, center, quat, ShapeParam, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Sphere:
			UtilOverlap::SphereOverlapByChannel(overlapResults, world, center, ShapeParam.X, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Capsule:
			UtilOverlap::CapsuleOverlapByChannel(overlapResults, world, center, quat, ShapeParam.X, ShapeParam.Y, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Sector:
			// meshTrans.GetUnitAxis(EAxis::X) 는 MeshComp의 로컬 X축을 구함
			UtilOverlap::SectorOverlapByChannel(overlapResults, world, FTransform(quat, center), ShapeParam.X, ShapeParam.Y, ShapeParam.Z, BodyInstance.GetObjectType(), params, responseParams, bDrawDebug, DebugColor, DebugTime);
			break;
		}
		
		// 결과를 처리한다
		// TODO : 으엑
		for (const FOverlapResult& overlapResult : overlapResults)
		{
			_ProcessOverlapActor(overlapResult.GetActor());
			// if(OnBeginDetectDelegate.IsBound())
			// 	OnBeginDetectDelegate.Broadcast(FDetectResult(FVector(0.f), overlapResult.GetComponent(), overlapResult.GetActor()));

			_SpawnNiagara(owner, overlapResult);
		}
	}
}

/**
 *	Overlap 결과를 처리
 *	TODO : 뭘 어떻게 처리할까.. ?
 */
void UAnimNotify_OverlapCheck::_ProcessOverlapActor(const AActor* InActor) const
{
	if(!IsValid(InActor))
		return;
	
	// 여기에서 로직 처리
	LOG_SCREEN(FColor::Purple, TEXT("%s Overlap!"), *InActor->GetName());

	
}

/**
 *	Effect를 처리한다.
 *	@param InInstigator : 오버랩을 시전한 액터
 *	@param InResult : 오버랩 결과
 */
void UAnimNotify_OverlapCheck::_SpawnNiagara(const AActor* InInstigator, const FOverlapResult& InResult) const
{
	const APawn* owner = Cast<APawn>(InInstigator);
	if(!IsValid(owner))
		return;

	// TODO : AIController를 고려해야함
	AR4PlayerController* controller = Cast<AR4PlayerController>(owner->GetController());
	if(!IsValid(controller))
		return;
	
	// TODO : Cascade를 추가해야 에셋들을 좀 쓰겄는디..;
	for(const auto& [particle, type] : Particles)
	{
		FVector closestPoint;
		switch (type)
		{
		case EOverlapEffectType::ClosestPoint:
			InResult.GetComponent()->GetClosestPointOnCollision(InInstigator->GetActorLocation(), closestPoint);
			
			UtilEffect::SpawnNiagaraAtLocation_Local(particle, closestPoint, owner->GetActorRotation(), FVector(1.f), controller->GetWorld());
			controller->ServerRPC_NotifySpawnNiagaraAtLocation(particle, closestPoint, owner->GetActorRotation(), FVector(1.f), controller->GetWorld());
			break;

		case EOverlapEffectType::Instigator:
			UtilEffect::SpawnNiagaraAttached_Local(particle, owner->GetRootComponent(), EName::None, FVector(0.f), owner->GetActorRotation());
			controller->ServerRPC_NotifySpawnNiagaraAttached(particle, owner->GetRootComponent(), EName::None, FVector(0.f), owner->GetActorRotation());
			break;

		case EOverlapEffectType::OverlappedActor:
			if(InResult.GetActor() == nullptr)
				break;
			
			UtilEffect::SpawnNiagaraAttached_Local(particle, InResult.GetActor()->GetRootComponent(), EName::None, FVector(0.f), InResult.GetActor()->GetActorRotation());
			controller->ServerRPC_NotifySpawnNiagaraAttached(particle, InResult.GetActor()->GetRootComponent(), EName::None, FVector(0.f), InResult.GetActor()->GetActorRotation());
			break;
		}
	}
}
