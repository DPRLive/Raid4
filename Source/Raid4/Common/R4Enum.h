﻿#pragma once

/**
 *  여러 곳에서 사용할 Enum 값 모음
 */

/**
 *  Player가 Skill 발동 시 사용할 index
 */
UENUM( BlueprintType )
enum class EPlayerSkillIndex : uint8
{
    Attack = 0	UMETA( DisplayName = "기본 공격" ),
	Skill1		UMETA( DisplayName = "1번 스킬" ),
    Skill2		UMETA( DisplayName = "2번 스킬" ),
	Skill3		UMETA( DisplayName = "3번 스킬" ),
	MAX			UMETA( Hidden )
};
ENUM_RANGE_BY_COUNT( EPlayerSkillIndex, EPlayerSkillIndex::MAX );

/**
 *  Overlap 시 출력할 이펙트의 타입
 */
UENUM( BlueprintType )
enum class EOverlapEffectType : uint8
{
	OverlappedActor		UMETA( DisplayName = "오버랩 된 액터" ),
	Instigator			UMETA( DisplayName = "시전자에게 출력" ),
	ClosestPoint		UMETA( DisplayName = "시전자를 기준으로 한 오버랩 된 위치" )
};

/**
 * Stat 계산 시 어떤것을 피연산자로 할 것 인지
 */
UENUM( BlueprintType )
enum class EStatOperandType : uint8
{
	Base			UMETA( DisplayName = "기본 스탯" ),
	Total			UMETA( DisplayName = "스탯 총합" ),
	Current			UMETA( DisplayName = "현재 스탯" ),
};

/**
 * 연산자 타입
 */
UENUM( BlueprintType )
enum class EOperatorType : uint8
{
	Add				UMETA( DisplayName = "더하기" ),
	Multiply		UMETA( DisplayName = "곱하기" ),
};

/**
 *  가해자, 피해자 타입
 */
UENUM( BlueprintType )
enum class ETargetType : uint8
{
	Instigator		UMETA( DisplayName = "가해자 객체" ),
	Victim			UMETA( DisplayName = "피해자 객체" ),
};

/**
 *  GameplayTag간 쿼리 시 사용할 쿼리 타입
 */
UENUM( BlueprintType )
enum class EGameplayTagQueryType : uint8
{
	Match			UMETA( DisplayName = "부모로 사용되어도 일치" ),
	MatchExact		UMETA( DisplayName = "완전히 일치" ),
};

/**
 * Notify의 Type들을 지정.
 */
UENUM()
enum class ER4AnimNotifyType : uint8
{
	None			UMETA( DisplayName = "None" ),
	Detect			UMETA( DisplayName = "탐지" ),
	ComboInputTest	UMETA( DisplayName = "콤보 입력 확인" ),
	ApplyBuff		UMETA( DisplayName = "특정 버프 적용" ),
	MAX				UMETA( Hidden )
};

/**
 * Force Movement시 사용할 방법.
 */
UENUM( BlueprintType )
enum class ER4ForceMoveType : uint8
{
	Linear			UMETA( DisplayName = "목적지까지 선형이동" ),
	CurveVector		UMETA( DisplayName = "CurveVector 이용" ),
	None			UMETA( Hidden )
};

/**
 * Network 정책 설정 시 사용할 Bitflag
 */
UENUM( BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true") )
enum class ER4NetworkFlag : uint8
{
	None		=  0		UMETA( Hidden ),
	Server		= (1 << 0)	UMETA( DisplayName = "Server" ),
	Local		= (1 << 1)	UMETA( DisplayName = "Locally Control" ),
	Simulated 	= (1 << 2)	UMETA( DisplayName = "Simulated Client" ),
	Max			= (1 << 3)	UMETA( Hidden )
};
ENUM_CLASS_FLAGS(ER4NetworkFlag);

/**
 * Network 정책 설정 시 사용할 Bitflag ( No Simulated Flag )
 */
UENUM( BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true") )
enum class ER4NetworkFlag_NoSimulated : uint8
{
	None		= 0			UMETA( Hidden ),
	Server		= (1 << 0)	UMETA( DisplayName = "Server" ),
	Local		= (1 << 1)	UMETA( DisplayName = "Locally Control" ),
	Max			= (1 << 2)	UMETA( Hidden )
};
ENUM_CLASS_FLAGS(ER4NetworkFlag_NoSimulated);
