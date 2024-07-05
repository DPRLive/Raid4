#pragma once

/**
 *  DataAsset을 편하게 가져오게 해주는 Helper
 */
namespace R4DataAssetHelper
{
	/**
    *  이름으로 DataAsset을 가져온다. 단, /Game/Raid4/Data/DataAsset에 있어야 함.
    */
    template<typename T>
    const TObjectPtr<T> GetDataAsset(const FString& InAssetName)
    {
    	return LoadObject<T>(nullptr, *(UtilPath::GetDataAssetPath(InAssetName)));
    }
}

