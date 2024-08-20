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
 *	@return : 추가 된 방어막이 삭제 될 때 알릴 Delegate.
 */
FSimpleDelegate* UR4ShieldComponent::AddShield(UObject* InProvideObj, float InValue)
{
	// 음수의 쉴드량은 처리하지 않음.
	if(InValue < 0.f)
	{
		LOG_WARN(R4Log, TEXT("Try to add negative shield amount! [%f]"), InValue);
		return nullptr;
	}
	
	// 중복 추가 방지, 찾았다면 제거
	if(FShieldListNode* shield = _FindShieldByObject(InProvideObj))
	{
		TotalShield -= shield->GetValue().ShieldAmount;
		
		shield->GetValue().OnRemoveShieldDelegate.ExecuteIfBound();
		Shields.RemoveNode(shield);
	}

	// 방어막 추가
	Shields.AddTail(FShieldInfo(InProvideObj, InValue));
	TotalShield += InValue;

	_OnRep_TotalShield();

	return (Shields.GetTail() ? &(Shields.GetTail()->GetValue().OnRemoveShieldDelegate) : nullptr);
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
		auto& [nowObj, nowShield, nowDelegate] = shield->GetValue();
		
		// 소모해야 하는 양이 더 많거나 같은 경우
		if(InValue > nowShield || FMath::IsNearlyEqual(InValue, nowShield))
		{
			InValue -= nowShield;
			totalConsume += nowShield;

			// 쉴드 삭제
			nowDelegate.ExecuteIfBound();
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
	if(FShieldListNode* shield = _FindShieldByObject(InProvideObj))
	{
		TotalShield -= shield->GetValue().ShieldAmount;

		shield->GetValue().OnRemoveShieldDelegate.ExecuteIfBound();
		Shields.RemoveNode(shield);

		_OnRep_TotalShield();

		return true;
	}

	return false;
}

/**
 *  Shield Node 찾기
 */
UR4ShieldComponent::FShieldListNode* UR4ShieldComponent::_FindShieldByObject(UObject* InProvideObj) const
{
	FShieldListNode* node = Shields.GetHead();
	
	while ( node != nullptr )
	{
		if ( node->GetValue().ShieldProvideObj == InProvideObj )
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
