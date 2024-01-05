#pragma once

/**
* 여러 경로들을 쉽게 가져오기 위한 Util namespace
*/
namespace UtilPath
{
	/**
	* 이름으로 데이터 테이블의 경로를 반환. /Game/Raid4/Data/DataTable에 있어야 함.
	*/
	FORCEINLINE const FString GetDataTablePath(const FString& InFileName)
	{
		const FString path = FString::Printf(TEXT("/Game/Raid4/Data/DataTable/%s.%s"), *InFileName, *InFileName);
		return path;
	}

	/**
    * 이름으로 데이터 에셋의 경로를 반환. /Game/Raid4/Data/DataAsset에 있어야 함.
    */
	FORCEINLINE const FString GetDataAssetPath(const FString& InFileName)
	{
		const FString path = FString::Printf(TEXT("/Game/Raid4/Data/DataAsset/%s.%s"), *InFileName, *InFileName);
		return path;
	}
}

