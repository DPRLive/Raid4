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
    Skill1 UMETA( ToolTip = "1번 스킬" ),
	Skill2 UMETA( ToolTip = "2번 스킬" ),
    Skill3 UMETA( ToolTip = "3번 스킬" ),
	Skill4 UMETA( ToolTip = "4번 스킬" ),
	Skill5 UMETA( ToolTip = "5번 스킬" ),
	Max
};
ENUM_RANGE_BY_COUNT( ESkillIndex, ESkillIndex::Max );