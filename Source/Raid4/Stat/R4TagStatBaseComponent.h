// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4StatStruct.h"
#include "R4TagStatBaseComponent.generated.h"

/**
 * GameplayTag로 쿼리하는 기능을 제공하는 StatComp.
 * ( Stat Comp가 해당 '000' 스탯을 가지고 있을지 없을지 모를때 유용할 것이야!!)
 */
UCLASS( Abstract )
class RAID4_API UR4TagStatBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4TagStatBaseComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Stat 정보를 재설정 (Base Value로 세팅)
	virtual void ResetStat() PURE_VIRTUAL(UR4StatBaseComponent::ResetStat, );
	
	// Tag <-> Stat Getter, Tag와 Stat이 Bind가 된 상태이어야 쿼리 가능,
	// 해당 Stat Comp가 Tag에 맞는 스탯을 들고 있을지 없을지 명확하지 않을때, 유용하게 사용 가능
	template<typename T>
	T* GetStatByTag(const FGameplayTag& InTag);

protected:
	// Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
	void BindTagToStat(const FGameplayTag& InTag, FR4StatInfo& InStatRef);
	void BindTagToStat(const FGameplayTag& InTag, FR4CurrentStatInfo& InStatRef);
	
	// Tag <-> Stat 바인드 Clear
	void ClearTagStats();

private:
	// 일반 Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4StatInfo*> TagStats;

	// Current Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4CurrentStatInfo*> TagCurrentStats;
};

/**
 * Tag <-> Stat Getter, Tag와 Stat이 Bind가 된 상태이어야 쿼리 가능,
 * 해당 Stat Comp가 Tag에 맞는 스탯을 들고 있을지 없을지 명확하지 않을때, 유용하게 사용 가능
 */
template <typename T>
T* UR4TagStatBaseComponent::GetStatByTag(const FGameplayTag& InTag)
{
	if(auto value = TagCurrentStats.Find(InTag))
		return *value;

	// derived = Base 방지
	if constexpr (std::is_same_v<FR4StatInfo, T>)
	{
		if(auto value = TagStats.Find(InTag))
			return *value;
	}
	
	return nullptr;
}
