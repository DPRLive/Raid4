#pragma once

#include "R4Struct.generated.h"

/**
 * Common한 구조체 모음
 */

// FGameplayTag와 float value를 가지는 struct
USTRUCT( BlueprintType )
struct FR4TagValue
{
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

/**
 * Common한 구조체 모음
 */

/**
 * Area Of Effect. Material과 Texture에서 실제 범위 표시 비율을 설정
 */
USTRUCT( BlueprintType )
struct FR4AreaOfEffect
{
	GENERATED_BODY()

	FR4AreaOfEffect()
	: AOEDecal( nullptr )
	, RangeRatio( 0.f )
	{ }

	// Texture의 실제 범위 표시 비율을 고려한, 표시하고 싶은 InActualRadius에 맞춘 Decal의 크기 반환.
	float GetDecalSizeByActualRadius( float InActualRadius ) const
	{
		return InActualRadius / RangeRatio;
	}
	
	// 범위 표시 Decal
	UPROPERTY( EditDefaultsOnly )
	TObjectPtr<UMaterialInterface> AOEDecal;

	// Texture에서 중점으로부터 실제 범위 표시에 해당하는 비율.
	UPROPERTY( EditDefaultsOnly, meta = (ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f) )
	float RangeRatio;
};