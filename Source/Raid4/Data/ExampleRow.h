#pragma once

#include "R4RowBase.h"
#include "ExampleRow.generated.h"

// 데이터 테이블 예시
USTRUCT()
struct FExampleRow : public FR4RowBase
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString Good = TEXT("");
};

GENERATE_DT_PTR_H( FExampleRow );