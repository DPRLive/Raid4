#pragma once

#include "R4Struct.generated.h"

/**
 * Common한 구조체 모음
 */

// FGameplayTag와 float value를 가지는 struct
USTRUCT( BlueprintType )
struct FR4TagValue
{
public:
	GENERATED_BODY()

	FR4TagValue()
	 : Tag(FGameplayTag::EmptyTag)
	 , Value(0.f)
	{}

	FR4TagValue(const FGameplayTag& InTag, float InValue = 0.f)
	 : Tag(InTag)
	 , Value(InValue)
	{}
 
	UPROPERTY( EditDefaultsOnly )
	FGameplayTag Tag;
 
	UPROPERTY( EditDefaultsOnly )
	float Value;
};