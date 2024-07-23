// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4StatBaseComponent.generated.h"

struct FR4StatData;
struct FR4ConsumableStatData;

/**
 * Stat Component의 Base가 되는 클래스
 * GameplayTag로 관리하는 관련 기능을 제공
 */
UCLASS( Abstract )
class RAID4_API UR4StatBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4StatBaseComponent();

protected:
	virtual void BeginPlay() override;

	// Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
	void BindTagToStat(const FGameplayTag& InTag, FR4StatData& InStatRef);
	void BindTagToStat(const FGameplayTag& InTag, FR4ConsumableStatData& InStatRef);

	// Tag <-> Stat Getter
	FR4StatData* GetStatByTag(const FGameplayTag& InTag);
	FR4ConsumableStatData* GetConsumableStatByTag(const FGameplayTag& InTag);
	
	// Tag <-> Stat 바인드 Clear
	void ClearTagStats();
	
	// 일반 Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4StatData*> TagStats;

	// 소모형 Stat Tag 쿼리용 TMap
	TMap<FGameplayTag, FR4ConsumableStatData*> TagConsumableStats;
};
