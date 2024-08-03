// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include "PoolableInterface.h"
#include "PoolableActor.generated.h"

/**
 * ActorPool에 의해 Pooling 될 수 있는 Actor를 정의 시 사용
 * dynamic한 pool이므로 자주 생성 / 삭제가 필요한 경우에만 사용!
 * (처음 생성 시 오버헤드가 더 들음)
 */
UCLASS( Abstract )
class RAID4_API APoolableActor : public AActor, public IPoolableInterface
{
	GENERATED_BODY()
	
public:	
	APoolableActor();
	
	// Server에서 bHidden 시 Client side에서 Destroy 당하지 않도록 bHidden && Disable Collision 일 경우에도 Relevant를 가지도록 설정
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// Actor를 Pool에 반납하기 위해 비활성화
	virtual void DeactivateActor();

	// Actor를 Pool에서 꺼내기 위해 활성화
	virtual void ActivateActor();

	// Actor를 Pool에서 사용하기 위한 꺼낸 후 로직 처리
	virtual void PostInitPoolObject() override {}

	// Actor를 Pool에서 반납하기 위한 반납 전 로직 처리
	virtual void PreReturnPoolObject() override {}
	
	// Actor가 현재 활성화 상태인지 확인
	FORCEINLINE bool IsActivate() const { return bActivate; }

protected:
	// Collision과 Tick을 자식 컴포넌트까지 모두 비활성화.
	void DisableCollisionAndTick();

	// Collision과 Tick을 자식 컴포넌트까지 원래 CDO (UCLASS) 대로 복구
	void ResetCollisionAndTick();

	// OnRep_IsActivate
	UFUNCTION()
	void OnRep_Activate();

private:
	// Actor가 현재 활성화 상태인지 확인
	UPROPERTY(ReplicatedUsing = OnRep_Activate)
	uint8 bActivate:1;
};
