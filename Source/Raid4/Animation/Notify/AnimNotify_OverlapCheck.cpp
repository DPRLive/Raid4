// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OverlapCheck.h"
#include "../../Util/UtilOverlap.h"

#include <Components/SkeletalMeshComponent.h>

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
	
	// TODO : 내가 직접 움직이는 Proxy에서만 발동하도록 설정 필요
	if (const UWorld* world = MeshComp->GetWorld())
	{
		// Relative Location을 더한 위치, 회전을 구함
		// TODO : UI용 마우스 위치 버전을 만들어야해! 아하하! 마우스로 회전 기능도 만들어야해!
		const FTransform& meshTrans = MeshComp->GetComponentTransform();
		const FVector center = meshTrans.TransformPosition(RelativeLoc);
		const FQuat quat = meshTrans.GetRotation() * RelativeRot.Quaternion();
		
		FCollisionQueryParams params;
		params.AddIgnoredActor(MeshComp->GetOwner()); // 나는 무시
		TArray<FOverlapResult> overlapResults;
		
		switch (Shape) // 모양에 맞춰 overlap 체크
		{
		case EOverlapShape::Box:
			UtilOverlap::BoxOverlapByProfile(overlapResults, world, center, quat, ShapeParam, Profile, params, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Sphere:
			UtilOverlap::SphereOverlapByProfile(overlapResults, world, center, ShapeParam.X, Profile, params, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Capsule:
			UtilOverlap::CapsuleOverlapByProfile(overlapResults, world, center, quat, ShapeParam.X, ShapeParam.Y, Profile, params, bDrawDebug, DebugColor, DebugTime);
			break;
		case EOverlapShape::Sector:
			// meshTrans.GetUnitAxis(EAxis::X) 는 MeshComp의 로컬 X축을 구함
			UtilOverlap::SectorOverlapByProfile(overlapResults, world, FTransform(quat, center), ShapeParam.X, ShapeParam.Y, ShapeParam.Z, Profile, params, bDrawDebug, DebugColor, DebugTime);
			break;
		}
		
		// 결과를 처리한다
		for (const FOverlapResult& overlapResult : overlapResults)
		{
			_ProcessOverlapActor(overlapResult.GetActor());
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
