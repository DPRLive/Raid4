// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>

#include "R4ShieldComponent.generated.h"

/**
 * 방어막 기능을 부여해주는 컴포넌트.
 * TODO : Test, UI Bind
 */
UCLASS(ClassGroup=(Shield), meta=(BlueprintSpawnableComponent))
class RAID4_API UR4ShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UR4ShieldComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// InValue 만큼의 Shield를 추가. InProvideObj에 따라서 방어막을 부여한 객체를 구분 가능
	void AddShield(UObject* InProvideObj, float InValue);

	// InValue 만큼의 Shield를 소모.
	float ConsumeShield(float InValue);
	
	// InProvideObj가 부여한 Shield를 제거
	bool RemoveShield(UObject* InProvideObj);

	// 현재 총 방어막 수치를 return
	FORCEINLINE float GetTotalShield() const { return TotalShield; }

private:
	// Shields 의 Node 타입
	using FShieldListNode = TDoubleLinkedList<TTuple<TWeakObjectPtr<>, float>>::TDoubleLinkedListNode;
	
	// Shield Node 찾기
	FShieldListNode* _FindShieldByObject(UObject* InProvideObj) const;
	
	// Total Shield가 변경되면, Broadcast
	UFUNCTION()
	void _OnRep_TotalShield();

public:
	// 방어막 총 수치 변경시 broadcast.
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeTotalShieldDelegate, float);
	FOnChangeTotalShieldDelegate OnChangeTotalShieldDelegate;
	
private:
	// 현재 적용된 방어막
	TDoubleLinkedList<TPair<TWeakObjectPtr<UObject>, float>> Shields;

	// 현재 방어막의 총량
	UPROPERTY(Transient, VisibleInstanceOnly, ReplicatedUsing = _OnRep_TotalShield )
	float TotalShield;
};
