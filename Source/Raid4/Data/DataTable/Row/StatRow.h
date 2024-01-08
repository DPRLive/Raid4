#pragma once

#include "../R4RowBase.h"
#include "StatRow.generated.h"

/**
 * 객체의 스탯을 정의할 Row
 */
USTRUCT()
struct FStatRow : public FR4RowBase
{
	GENERATED_BODY()
	
};

GENERATE_DT_PTR_H( FStatRow );