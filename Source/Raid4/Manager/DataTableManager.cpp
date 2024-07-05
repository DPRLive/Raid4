#include "DataTableManager.h"

#include "../Data/ExampleRow.h"
#include "../Stat/R4StatRow.h"
#include "../Character/R4CharacterRow.h"

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
        if(clearFunc)
        	clearFunc();
    }
    ClearFuncs.Empty();
}

/**
 * 데이터 테이블들을 로드한다.
 */
void FDataTableManager::_LoadDataTableAll()
{
	LOAD_DATATABLE( FExampleRow, ExampleRow, TEXT("DT_Example") );
	LOAD_DATATABLE( FR4StatRow, R4StatRow, TEXT("DT_CharacterBaseStat") );
	LOAD_DATATABLE( FR4CharacterRow, R4CharacterRow, TEXT("DT_Character") );
}

