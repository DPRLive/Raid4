// Fill out your copyright notice in the Description page of Project Settings.


#include "R4ShieldComponent.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4ShieldComponent)

UR4ShieldComponent::UR4ShieldComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	TotalShield = 0.f;
}

/**
 *	GetLifetimeReplicatedProps
 */
void UR4ShieldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UR4ShieldComponent, TotalShield);
}

void UR4ShieldComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Clear();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *	InValue 만큼의 Shield를 추가. InProvideObj에 따라서 방어막을 부여한 객체를 구분 가능
 *	@param InProvideObj : 방어막을 부여할 객체
 *	@param InValue : 부여할 방어막 값
 */
void UR4ShieldComponent::AddShield(const UObject* InProvideObj, float InValue)
{
	// 음수의 쉴드량은 처리하지 않음.
	if(InValue < 0.f)
	{
		LOG_WARN(R4Log, TEXT("Try to add negative shield amount! [%f]"), InValue);
		return;
	}

	// 방어막 추가
	Shields.AddTail({InProvideObj, InValue});
	TotalShield += InValue;

	_OnRep_TotalShield();
}

/**
 *	InValue 만큼의 Shield를 소모.
 *	@param InValue : 소모할 방어막 값
 *	@return : 총 소모한 방어막 값
 */
float UR4ShieldComponent::ConsumeShield(float InValue)
{
	// 음수의 쉴드량은 처리하지 않음.
	if(InValue < 0.f)
	{
		LOG_WARN(R4Log, TEXT("Try to consume negative shield amount! [%f]"), InValue);
		return 0.f;
	}
	
	float totalConsume = 0.f;

	FShieldListNode* shield = Shields.GetHead();
	while ( shield != nullptr && !FMath::IsNearlyZero(InValue) )
	{
		auto& [nowObj, nowShield] = shield->GetValue();
		
		// 소모해야 하는 양이 더 많거나 같은 경우
		if(InValue > nowShield || FMath::IsNearlyEqual(InValue, nowShield))
		{
			InValue -= nowShield;
			totalConsume += nowShield;
			
			Shields.RemoveNode(shield);
			shield = Shields.GetHead();
			continue;
		}

		// 아닌 경우 단순 소모
		nowShield -= InValue;
		totalConsume += InValue;
		
		InValue = 0.f;
	}

	// 방어막 소모 발생시
	if(!FMath::IsNearlyZero(totalConsume))
	{
		TotalShield -= totalConsume;
		_OnRep_TotalShield();
	}
	
	return totalConsume;
}

/**
 *	InProvideObj가 부여한 Shield를 제거
 *	@param InProvideObj : 제거할 방어막을 부여한 객체
 *	@return : 성공 여부
 */
bool UR4ShieldComponent::RemoveShield(const UObject* InProvideObj)
{
	// 찾았다면 제거
	if(FShieldListNode* shield = _FindShieldByObject(InProvideObj))
	{
		TotalShield -= shield->GetValue().Value;
		Shields.RemoveNode(shield);

		_OnRep_TotalShield();

		return true;
	}

	return false;
}

/**
 *	InProvideObj가 부여한 모든 Shield를 제거
 *	@param InProvideObj : 제거할 방어막을 부여한 객체
 *	@return : 성공 여부
 */
bool UR4ShieldComponent::RemoveShieldAll(const UObject* InProvideObj)
{
	bool bRemove = false;
	
	FShieldListNode* node = Shields.GetHead();
	
	while ( node != nullptr )
	{
		if ( node->GetValue().Key == InProvideObj )
		{
			// 삭제할 노드 캐싱
			FShieldListNode* targetNode	= node;
			node = node->GetNextNode();

			// Remove
			TotalShield -= targetNode->GetValue().Value;
			Shields.RemoveNode(targetNode);
			bRemove = true;
			
			continue;
		}

		node = node->GetNextNode();
	}

	if(bRemove)
		_OnRep_TotalShield();
	
	return bRemove;
}

/**
 *  Shield Comp 초기화.
 */
void UR4ShieldComponent::Clear()
{
	OnChangeTotalShieldDelegate.Clear();
	Shields.Empty();
	TotalShield = 0.f;
}

/**
 *  Shield Node 찾기
 */
UR4ShieldComponent::FShieldListNode* UR4ShieldComponent::_FindShieldByObject(const UObject* InProvideObj) const
{
	FShieldListNode* node = Shields.GetHead();
	
	while ( node != nullptr )
	{
		if ( node->GetValue().Key == InProvideObj )
			break;

		node = node->GetNextNode();
	}

	return node;
}

/**
 *  Total Shield가 변경되면, Broadcast
 */
void UR4ShieldComponent::_OnRep_TotalShield()
{
	if(OnChangeTotalShieldDelegate.IsBound())
		OnChangeTotalShieldDelegate.Broadcast(TotalShield);
}
