#pragma once

#include "TimeLimitChecker.h"

/**
 *  제한 시간을 리턴
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 *  @return : 제한시간, -1.f = 키에 맞는 값을 찾지 못함 
 */
template<typename Type>
float TTimeLimitChecker<Type>::GetTimeLimit(const Type& InKey) const
{
	if(auto it = TimeLimits.Find(InKey))
		return it->Key;

	return -1.f;
}

/**
 *  제한 시간을 다시 설정
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 *  @param InTimeLimitDuration : 새로운 제한 시간
 *  @return : 성공여부, false = 키에 맞는 값을 찾지 못함 
 */
template <typename Type>
bool TTimeLimitChecker<Type>::SetTimeLimit(const Type& InKey, float InTimeLimitDuration)
{
	if(auto it = TimeLimits.Find(InKey))
	{
		it->Key = InTimeLimitDuration;
		return true;
	}

	return false;
}

/**
 *  남은 제한 시간을 리턴
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 *  @param InServerTime : 현재 서버 시간.
 *  @return : 남은 제한 시간, -1.f = 키에 맞는 값을 찾지 못함 
 */
template <typename Type>
float TTimeLimitChecker<Type>::GetRemainingTime(const Type& InKey, float InServerTime) const
{
	// 서버시간 안줬으면 R4GetServerTimeSeconds 사용
	InServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);
	
	if(auto it = TimeLimits.Find(InKey))
	{
		float remainTime = it->Key + it->Value - InServerTime;
		return FMath::Clamp(remainTime, 0.f, remainTime);
	}

	return -1.f;
}

/**
 *  제한시간이 지났는지 리턴.
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 *  @param InServerTime : 현재 서버 시간.
 *  @return : 제한시간이 만료 되었는지 여부
 */
template <typename Type>
bool TTimeLimitChecker<Type>::IsTimeLimitExpired( const Type& InKey, float InServerTime ) const
{
	float remainingTime = GetRemainingTime( InKey, InServerTime );

	// 애초에 안걸려 있었거나 남은 제한 시간이 0인지 확인
	return (remainingTime < 0.f || FMath::IsNearlyZero(remainingTime));
}

/**
 *  새로운 Time 제한 Check 추가. 기존 key와 중복시 override
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 *  @param InTimeLimitDuration : 제한 시간
 *  @param InServerTime : 제한시간 체크의 기준이 되는 시작시간(서버).
 */
template <typename Type>
void TTimeLimitChecker<Type>::AddNewTimeCheck(const Type& InKey, float InTimeLimitDuration, float InServerTime)
{
	InServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);
	TimeLimits.Emplace(InKey, {InTimeLimitDuration, InServerTime});
}

/**
 *  시간 Time 제한 Check 제거.
 *  @param InKey : 제한 시간 적용 시 사용했던 키
 */
template <typename Type>
void TTimeLimitChecker<Type>::RemoveTimeCheck(const Type& InKey)
{
	TimeLimits.Remove(InKey);
}

/**
 *  InServerTime 기준으로 만료된 시간 제한들 삭제.
 */
template <typename Type>
void TTimeLimitChecker<Type>::ClearExpiredTimes( float InServerTime )
{
	InServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);

	for(auto it = TimeLimits.CreateIterator(); it; ++it)
	{
		// 제한 시간이 만료된 경우 제거.
		if( (it->Value.Key + it->Value.Value) < InServerTime )
			it.RemoveCurrentSwap();
	}
}

/**
 *  모든 제한시간 삭제
 */
template <typename Type>
void TTimeLimitChecker<Type>::Reset()
{
	TimeLimits.Empty();
}
