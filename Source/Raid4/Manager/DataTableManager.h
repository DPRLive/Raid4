#pragma once

#include "../Core/SingletonBase.h"
#include "../Data/DataTable/RowBase.h"

/**
 * 데이터 테이블을 편하게 등록하기 위한 매크로
 * FRowBase를 기반으로 만든 Type과 이름(접두사 한개 뺀거)를 넣어서 생성
 */
#define DECLARE_DATATABLE(Type, Name)							\
private:														\
    TMap<FPriKey, const Type*> Name##s;							\
public:															\
    const Type* Get##Name(FPriKey InPK) const					\
    {															\
		if(const Type* const *rowPtr = Name##s.Find( InPK ))	\
			return *rowPtr;										\
		return nullptr;											\
	}														

// 여기에 전방 선언 //
struct FExampleRow;
////////////////////

/**
 * 모든 데이터 테이블을 로드하여 관리하는 DataTable Manager (Singleton)
 */
class FDataTableManager : public TSingletonBase<FDataTableManager>
{
private:
	// 데이터 테이블을 선언 //
	DECLARE_DATATABLE( FExampleRow, ExampleRow );
	///////////////////////

public:
	FDataTableManager() = default;
	
	virtual ~FDataTableManager() override = default;

	// 데이터 초기화 (로드)
	virtual void InitSingleton() override;

	// 데이터 정리
	virtual void ClearSingleton() override;

private:
	// 데이터 테이블들을 로드한다.
	void _LoadDataTableAll();

	// 데이터 테이블을 로드한다. ( FRowBase를 상속받은 DT일때 템플릿을 활성화, 오버로딩을 위해 enable_if를 반환값으로)
	template<typename Type, typename RowMap>
	typename TEnableIf<TIsDerivedFrom<Type, FRowBase>::Value>::Type
	_LoadDataTable(RowMap& InRowMap, const FString& InFileName);

	// 데이터 테이블을 로드한다. ( FRowBase를 상속받은 DT가 아니면 경고를 띄움 )
	template<typename Type, typename RowMap>
	typename TEnableIf<!TIsDerivedFrom<Type, FRowBase>::Value>::Type
	_LoadDataTable(RowMap& InRowMap, const FString& InFileName);
	
	// 데이터 테이블을 로드 해제해주는 람다 함수들
	TArray<TFunction<void()>> ClearFuncs;
};

/**
 * 데이터 테이블을 로드한다. ( FRowBase를 상속받은 DT일때 템플릿을 활성화 )
 */
template <typename Type, typename RowMap>
typename TEnableIf<TIsDerivedFrom<Type, FRowBase>::Value>::Type
FDataTableManager::_LoadDataTable(RowMap& InRowMap, const FString& InFileName)
{
	UDataTable* dataTable = LoadObject<UDataTable>(nullptr, *UtilPath::GetDataTablePath(InFileName));
	if(dataTable == nullptr)
	{
		// TODO: Log 추가
		return;
	}

	// DT를 읽어와서 Map에 추가
	for (const auto& [rowName, rowData] : dataTable->GetRowMap())
	{
		if (const Type* data = reinterpret_cast<Type*>(rowData))
		{
			// Invalid PK (0) 인 경우 체크
			if(!ensureMsgf(data->PrimaryKey != DTConst::G_InvalidPK, *FString::Printf(TEXT("[%s]:%s, PK : %d is invalid number."), *InFileName, *rowName.ToString(), data->PrimaryKey)))
				continue;

			// PK 중복인 경우 체크
			if(!ensureMsgf(InRowMap.Find(data->PrimaryKey) == nullptr, *FString::Printf(TEXT("[%s]:%s, PK : %d is duplicate number."), *InFileName, *rowName.ToString(), data->PrimaryKey)))
				continue;

			InRowMap.Add(data->PrimaryKey, data);
		}
	}

	// DT 정리를 위해 정리 함수 준비
	ClearFuncs.Emplace([&InRowMap]()
	{
		InRowMap.Empty();
	});
	
	// TODO: Log 추가
}

/**
 * ( FRowBase를 상속받은 DT가 아니면 경고를 띄움 )
 */
template <typename Type, typename RowMap>
typename TEnableIf<!TIsDerivedFrom<Type, FRowBase>::Value>::Type
FDataTableManager::_LoadDataTable(RowMap& InRowMap, const FString& InFileName)
{
	ensureMsgf(false, TEXT("[%s] Error ! Data table must be derived from FRowbase."), *InFileName);
}
