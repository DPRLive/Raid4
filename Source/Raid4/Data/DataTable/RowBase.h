#pragma once

#include <Engine/DataTable.h>
#include "RowBase.generated.h"

/**
 * Data Table을 정수 형태의 Pk로 사용하기 위한 Row Base
 */
USTRUCT()
struct FRowBase : public FTableRowBase
{
	GENERATED_BODY()

	// DT의 행을 식별할 Primary Key
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 PrimaryKey = DTConst::G_InvalidPK;

	// 에디터에서 Pk 중복 검사
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};
