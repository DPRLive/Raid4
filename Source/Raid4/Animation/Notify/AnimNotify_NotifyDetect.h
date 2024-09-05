// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Detect/R4NotifyDetectInterface.h"

#include <Animation/AnimNotifies/AnimNotify.h>

#include "AnimNotify_NotifyDetect.generated.h"

/**
 * Detect할 타이밍을 알리는 Anim Notify
 */
UCLASS()
class RAID4_API UAnimNotify_NotifyDetect : public UAnimNotify, public IR4NotifyDetectInterface
{
	GENERATED_BODY()

public:
	UAnimNotify_NotifyDetect();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	// ~ Begin IR4NotifyDetectInterface
	virtual FSimpleDelegate& OnNotifyDetect( const AActor* InReqActor ) override;
	// ~ End IR4NotifyDetectInterface

private:
	// 탐지 시점을 알려주기 위한 탐지 요청 {객체, delegate} 캐싱
	TArray<TPair<TWeakObjectPtr<const AActor>, FSimpleDelegate>> CachedDetectable;
};
