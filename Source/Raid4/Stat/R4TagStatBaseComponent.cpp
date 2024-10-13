// Fill out your copyright notice in the Description page of Project Settings.


#include "R4TagStatBaseComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4TagStatBaseComponent)

UR4TagStatBaseComponent::UR4TagStatBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UR4TagStatBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UR4TagStatBaseComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	ClearStatTags();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *	Stat Tag Data Clear 
 */
void UR4TagStatBaseComponent::ClearStatTags()
{
	TagStats.Empty();
	TagCurrentStats.Empty();
}

/**
 *	Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
 */
void UR4TagStatBaseComponent::BindTagToStat(const FGameplayTag& InTag, FR4StatInfo& InStatRef)
{
	// 중복 check
	if(!ensureMsgf(TagStats.Find(InTag) == nullptr, TEXT("Stat GameplayTag duplicated. : [%s]"), *InTag.ToString()))
		return;

	TagStats.Emplace(InTag, &InStatRef);
}

/**
 *	Tag <-> Stat 바인드용 함수. Bind가 된 상태이어야 Tag로 쿼리가 가능
 */
void UR4TagStatBaseComponent::BindTagToStat(const FGameplayTag& InTag, FR4CurrentStatInfo& InStatRef)
{
	// 중복시 checks
	if(!ensureMsgf(TagCurrentStats.Find(InTag) == nullptr, TEXT("Stat GameplayTag duplicated. : [%s]"), *InTag.ToString()))
		return;

	TagCurrentStats.Emplace(InTag, &InStatRef);
}
