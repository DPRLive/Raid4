// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>

#include "R4ShieldComponent.generated.h"

/**
 * '걸린 방어막' 정보를 관리.
 */
USTRUCT()
struct FShieldInfo
{
	GENERATED_BODY()

	FShieldInfo(UObject* InProvideObj = nullptr, float InValue = 0.f)
	: ShieldProvideObj(InProvideObj)
	, ShieldAmount(InValue)
	, OnRemoveShieldDelegate(FSimpleDelegate())
	{ }
	
	// 방어막을 제공한 객체 정보
	TWeakObjectPtr<UObject> ShieldProvideObj;

	// 방어막 량
	float ShieldAmount;

	// 해당 방어막의 삭제를 알리는 Delegate.
	FSimpleDelegate OnRemoveShieldDelegate;
};

/**
 * 방어막 기능을 부여해주는 컴포넌트.
 * TODO : Test, UI Bind, 방어막 버프 제작 -> 중첩 방지 필요할 시에 UClass로 구분하면 딱 좋겠다 및 중첩 테스트, Deactive 관련 설명 주석 추가 ㄱㄱ
 */
UCLASS(ClassGroup=(Shield), meta=(BlueprintSpawnableComponent))
class RAID4_API UR4ShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UR4ShieldComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// InValue 만큼의 Shield를 추가. InProvideObj에 따라서 방어막을 부여한 객체를 구분, 동일 객체로부터 여러개의 방어막 중첩 불가
	FSimpleDelegate* AddShield(UObject* InProvideObj, float InValue);

	// InValue 만큼의 Shield를 소모.
	float ConsumeShield(float InValue);
	
	// InProvideObj가 부여한 Shield를 제거
	bool RemoveShield(UObject* InProvideObj);

	// 현재 총 방어막 수치를 return
	FORCEINLINE float GetTotalShield() const { return TotalShield; }

private:
	// Shields 의 Node 타입
	using FShieldListNode = TDoubleLinkedList<FShieldInfo>::TDoubleLinkedListNode;
	
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
	TDoubleLinkedList<FShieldInfo> Shields;

	// 현재 방어막의 총량
	UPROPERTY(Transient, VisibleInstanceOnly, ReplicatedUsing = _OnRep_TotalShield )
	float TotalShield;
};
