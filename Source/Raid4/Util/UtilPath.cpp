#include "UtilPath.h"

/**
 * 이름으로 데이터 테이블의 경로를 반환. /Game/Raid4/Data/DataTable에 있어야 함.
 */
const FString UtilPath::GetDataTablePath(const FString& InFileName)
{
	const FString path = FString::Printf(TEXT("/Game/Raid4/Data/DataTable/%s.%s"), *InFileName, *InFileName);
	return path;
}
