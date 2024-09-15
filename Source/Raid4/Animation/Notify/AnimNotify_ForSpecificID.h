// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4NotifyByIdInterface.h"

#include <Animation/AnimNotifies/AnimNotify.h>

#include "AnimNotify_ForSpecificID.generated.h"

/**
 * Anim Notify에 특정 Delegate를 구독하고 사용할때 MontageInstance ID로 구분하여
 * 특정 Delegate에만 알리는 Notify.
 * ( 같은 Montage를 쓰면 같은 Notify를 공유하게 되므로 단순 Delegate로 구독 후 사용 시
 * Anim을 Play하지 않은 객체에도 알림이 갈 수 있는 문제를 방지 )
 * Detect 타이밍을 보장하기 위해 Montage Tick Type을 Branching Point로 사용.
 */
UCLASS()
class RAID4_API UAnimNotify_ForSpecificID : public UAnimNotify, public IR4NotifyByIdInterface
{
	GENERATED_BODY()

public:
	UAnimNotify_ForSpecificID();

	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual bool CanBePlaced( UAnimSequenceBase* InAnimation ) const override;
#endif

	virtual void BranchingPointNotify( FBranchingPointNotifyPayload& InBranchingPointPayload ) override;
	
public:
	// ~ Begin IR4NotifyByIdInterface
	virtual FSimpleDelegate& OnNotify( int32 InKey ) override;
	virtual void UnbindNotify( int32 InKey ) override;
	virtual ER4AnimNotifyType GetNotifyType() const override { return NotifyType; }
	// ~ End IR4NotifyByIdInterface

private:
	// 해당 Notify의 Type을 지정
	UPROPERTY( EditAnywhere, meta = (AllowPrivateAccess = true) )
	ER4AnimNotifyType NotifyType;
	
	// 특정 ID 기반 Delegate에 notify를 알려주기 위한 { ID, Delegate } 캐싱
	TMap<int32, FSimpleDelegate> CachedRequest;
};
