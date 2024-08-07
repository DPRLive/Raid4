// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4StatData.h"
#include "R4StatBaseComponent.generated.h"

/**
 * Stat Component의 Base가 되는 클래스
 * GameplayTag로 쿼리하는 기능을 제공.
 * ( Stat Comp가 해당 '000' 스탯을 가지고 있을지 없을지 모를때 유용할 것이야!!)
 */
UCLASS( Abstract )
class RAID4_API UR4StatBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4StatBaseComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Stat 정보를 재설정 (Base Value로 세팅)
	virtual void ResetStat() PURE_VIRTUAL(UR4StatBaseComponent::ResetStat, );
	
	// Tag <-> Stat Getter, Tag와 Stat이 Bind가 된 상태이어야 쿼리 가능,
	// 해당 Stat Comp가 Tag에 맞는 스탯을 들고 있을지 없을지 명확하지 않을때, 유용하게 사용 가능
	template<typename T>
	T* GetStatByTag(const FGameplayTag& InTag);

protected:
	// Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
	void BindTagToStat(const FGameplayTag& InTag, FR4StatData& InStatRef);
	void BindTagToStat(const FGameplayTag& InTag, FR4ConsumableStatData& InStatRef);
	
	// Tag <-> Stat 바인드 Clear
	void ClearTagStats();

private:
	// 일반 Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4StatData*> TagStats;

	// 소모형 Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4ConsumableStatData*> TagConsumableStats;
};

/**
 * Tag <-> Stat Getter, Tag와 Stat이 Bind가 된 상태이어야 쿼리 가능,
 * 해당 Stat Comp가 Tag에 맞는 스탯을 들고 있을지 없을지 명확하지 않을때, 유용하게 사용 가능
 */
template <typename T>
T* UR4StatBaseComponent::GetStatByTag(const FGameplayTag& InTag)
{
	if(auto value = TagConsumableStats.Find(InTag))
		return *value;

	// derived = Base 방지
	if constexpr (std::is_same_v<FR4StatData, T>)
	{
		if(auto value = TagStats.Find(InTag))
			return *value;
	}
	
	return nullptr;
}
