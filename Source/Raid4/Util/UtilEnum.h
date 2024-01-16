#pragma once

#include <Misc\EnumRange.h>

/**
 *  enum을 쉽게 사용하게 해주는 여러가지 유틸
 */
namespace UtilEnum
{
	/**
	* Enum을 FString으로 바꿔주는 함수
	*/
	template <typename T>
	FString EnumToString(T InValue)
	{
		UEnum* enumClass = StaticEnum<T>();

		if (!ensure(enumClass))
			return FString();

		return enumClass->GetNameStringByValue(static_cast<uint8>(InValue));
	}
}


/**
 * Enum 자체를 String으로 변경해주는 매크로
 */
#define ENUM_TO_STRING( Type, Value ) UtilEnum::EnumToString<Type>( Value )

/**
 * Enum을 Range based for loop로 쓸 수 있게 해주는 매크로
 * ENUM_RANGE_BY_COUNT( Type, Type::Max )가 선언되어야 함. (두번째 인자는 Enum의 마지막(최대 개수)로 판별하는 기준)
 */
#define ENUM_RANGE( Type ) TEnumRange<Type>()