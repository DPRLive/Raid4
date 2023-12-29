#pragma once

#include <Engine/DataTable.h>
#include "RowBase.generated.h"

/**
 * Data Table을 정수 형태의 Pk로 사용하기 위한 Row Base
 */
USTRUCT()
struct RAID4_API FRowBase : public FTableRowBase
{
	GENERATED_BODY()

	// DT의 행을 식별할 Primary Key
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 PrimaryKey = DTConst::G_InvalidPK;

	// 에디터에서 Pk 중복 검사
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};

// DT를 쉽게 가져오기 위한 class header ( FRowBase 상속받은 클래스에 작성) 
#define GENERATE_DT_PTR_H(Name)							\
class Name##Ptr											\
{														\
private:												\
	FPriKey PK = DTConst::G_InvalidPK;					\
	const Name* Row = nullptr;							\
														\
public:													\
	Name##Ptr() = delete;								\
	Name##Ptr(FPriKey InPK);							\
	FPriKey GetPK() const { return PK; }				\
	const Name* operator->() const { return Row; }		\
	const Name& operator*() const { return *Row; }		\
	bool IsValid() const { return Row != nullptr; }		\
}

// DT를 쉽게 가져오기 위한 class Cpp ( FRowBase 상속받은 클래스 cpp에 작성 + DataTableManager.h 추가필요) 
#define GENERATE_DT_PTR_CPP(Name, RowName)																\
Name##Ptr::Name##Ptr(FPriKey InPK) : PK(InPK), Row(FDataTableManager::Get()->Get##RowName( InPK )) {}