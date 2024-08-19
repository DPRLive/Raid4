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

/**
 *	InValue 만큼의 Shield를 추가. InProvideObj에 따라서 방어막을 부여한 객체를 구분, 동일 객체로부터 여러개의 방어막 중첩 불가
 *	@param InProvideObj : 방어막을 부여할 객체, 동일 객체로부터 여러개의 방어막 중첩 불가
 *	@param InValue : 부여할 방어막 값
 */
void UR4ShieldComponent::AddShield(UObject* InProvideObj, float InValue)
{
	// 중복 추가 방지, 찾았다면 제거
	if(ShieldListNode* shield = _FindShieldByObject(InProvideObj))
	{
		TotalShield -= shield->GetValue().Value;
		Shields.RemoveNode(shield);
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
	float totalConsume = 0.f;

	ShieldListNode* shield = Shields.GetHead();
	while ( shield != nullptr || !FMath::IsNearlyZero(InValue) )
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
bool UR4ShieldComponent::RemoveShield(UObject* InProvideObj)
{
	// 찾았다면 제거
	if(ShieldListNode* shield = _FindShieldByObject(InProvideObj))
	{
		TotalShield -= shield->GetValue().Value;
		Shields.RemoveNode(shield);

		_OnRep_TotalShield();

		return true;
	}

	return false;
}

/**
 *  Shield Node 찾기
 */
UR4ShieldComponent::ShieldListNode* UR4ShieldComponent::_FindShieldByObject(UObject* InProvideObj) const
{
	ShieldListNode* node = Shields.GetHead();
	
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
