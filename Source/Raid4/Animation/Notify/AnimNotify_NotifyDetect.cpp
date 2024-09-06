// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_NotifyDetect.h"

#include <Components/SkeletalMeshComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_NotifyDetect)

UAnimNotify_NotifyDetect::UAnimNotify_NotifyDetect()
{
}

void UAnimNotify_NotifyDetect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(!IsValid(MeshComp))
		return;
	
	// Owner값으로 판단하여, Anim을 Play한 실제 Character에게만 알림 전달.
	for(auto it = CachedDetectable.CreateIterator(); it; ++it)
	{
		// Invalid한 객체에 대한 element는 삭제
		if(!(it->Key.IsValid()))
		{
			it.RemoveCurrentSwap();
			continue;
		}

		if(it->Key.Get() == MeshComp->GetOwner())
		{
			it->Value.ExecuteIfBound();
			return;
		}
	}
}

/**
 *	탐지 시점을 알려주는 Delegate를 반환
 *	@param InReqActor : 탐지 타이밍을 알고 싶어하는 Actor 또는 객체의 Owner(Actor). nullptr은 넣어도 나중에 알림을 받을 수 없으니 주의!
 */
FSimpleDelegate& UAnimNotify_NotifyDetect::OnNotifyDetect( const AActor* InReqActor )
{
	for(auto it = CachedDetectable.CreateIterator(); it; ++it)
	{
		// Invalid한 객체에 대한 element는 삭제
		if(!(it->Key.IsValid()))
		{
			it.RemoveCurrentSwap();
			continue;
		}

		if(it->Key.Get() == InReqActor)
			return it->Value;
	}

	// 없으면 생성
	CachedDetectable.Emplace(InReqActor, FSimpleDelegate());
	return CachedDetectable.Last().Value;
}