#include "DataTableManager.h"

#include "../Data/DataTable/Row/ExampleRow.h"
#include "../Data/DataTable/Row/StatRow.h"
#include "../Data/DataTable/Row/CharacterRow.h"

/**
 * 데이터 초기화 (로드)
 */
void FDataTableManager::InitSingleton()
{
	_LoadDataTableAll();
}

/**
 * 데이터 정리
 */
void FDataTableManager::ClearSingleton()
{
    for(const auto& clearFunc : ClearFuncs)
    {
        if(clearFunc) clearFunc();
    }
    ClearFuncs.Empty();
}

/**
 * 데이터 테이블들을 로드한다.
 */
void FDataTableManager::_LoadDataTableAll()
{
	_LoadDataTable<FExampleRow>(ExampleRows, TEXT("DT_Example"));
	_LoadDataTable<FStatRow>(StatRows, TEXT("DT_CharacterStat"));
	_LoadDataTable<FCharacterRow>(CharacterRows, TEXT("DT_Character"));
}
