#pragma once

/**
 *  여러 곳에서 사용할 Enum 값 모음
 */

/**
 *  Skill 발동 시 사용할 index가 되는 타입
 */
UENUM( BlueprintType )
enum class ESkillIndex : uint8
{
    Skill1 = 0 UMETA( DisplayName = "1번 스킬" ),
	Skill2	   UMETA( DisplayName = "2번 스킬" ),
    Skill3	   UMETA( DisplayName = "3번 스킬" ),
	Skill4     UMETA( DisplayName = "4번 스킬" ),
	Skill5     UMETA( DisplayName = "5번 스킬" ),
	None
};
ENUM_RANGE_BY_COUNT( ESkillIndex, ESkillIndex::None );

/**
 *  Overlap 체크시 사용할 모양
 */
UENUM( BlueprintType )
enum class EOverlapShape : uint8
{
	Box			UMETA( DisplayName = "박스" ),
	Sphere		UMETA( DisplayName = "구" ),
	Capsule		UMETA( DisplayName = "캡슐" ),
	Sector		UMETA( DisplayName = "부채꼴" )
};

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