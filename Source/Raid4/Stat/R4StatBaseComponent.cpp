// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatBaseComponent)

UR4StatBaseComponent::UR4StatBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UR4StatBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *	Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
 */
void UR4StatBaseComponent::BindTagToStat(const FGameplayTag& InTag, FR4StatData& InStatRef)
{
	// 중복시 Warning
	if(TagStats.Find(InTag))
	{
		LOG_WARN(R4Stat, TEXT("Stat GameplayTag duplicated. : [%s]"), *InTag.ToString());
		return;
	}

	TagStats.Emplace(InTag, &InStatRef);
}

/**
 *	Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
 */
void UR4StatBaseComponent::BindTagToStat(const FGameplayTag& InTag, FR4ConsumableStatData& InStatRef)
{
	// 중복시 Warning
	if(TagConsumableStats.Find(InTag))
	{
		LOG_WARN(R4Stat, TEXT("Stat GameplayTag duplicated. : [%s]"), *InTag.ToString());
		return;
	}

	TagConsumableStats.Emplace(InTag, &InStatRef);
}

/**
 *	Tag <-> Stat Getter
 */
FR4StatData* UR4StatBaseComponent::GetStatByTag(const FGameplayTag& InTag)
{
	if(auto value = TagStats.Find(InTag))
		return *value;

	return nullptr;
}

/**
 *	Tag <-> Stat Getter
 */
FR4ConsumableStatData* UR4StatBaseComponent::GetConsumableStatByTag(const FGameplayTag& InTag)
{
	if(auto value = TagConsumableStats.Find(InTag))
		return *value;

	return nullptr;
}

/**
 *	Tag <-> Stat 바인드 Clear
 */
void UR4StatBaseComponent::ClearTagStats()
{
	TagStats.Empty();
	TagConsumableStats.Empty();
}
