#pragma once

/**
 * 특정 시간을 기록 후
 * 현재 시간과 비교할 때 사용하기 좋은 TTimeLimitChecker.
 * Tick을 굳이 사용해야 할 필요가 없을 때 사용하기 아주 좋지만
 * Tick을 사용하지 않기에 delegate가 없음
 */
template <typename Type>
class TTimeLimitChecker
{
public:
	TTimeLimitChecker() = default;
	
	// 제한 시간을 리턴
	float GetTimeLimit(const Type& InKey) const;
	
	// 제한 시간을 다시 설정
	bool SetTimeLimit(const Type& InKey, float InTimeLimitDuration);
	
	// 남은 제한 시간을 리턴
	float GetRemainingTime(const Type& InKey, float InServerTime = -1.f) const;
	
	// 제한시간이 지났는지 리턴
	bool IsTimeLimitExpired( const Type& InKey, float InServerTime = -1.f ) const;
	
	// 새로운 Time 제한 Check 추가. 기존 key와 중복시 override
	void AddNewTimeCheck(const Type& InKey, float InTimeLimitDuration, float InServerTime = -1.f);

	// 시간 Time 제한 Check 제거.
	void RemoveTimeCheck(const Type& InKey);

	// InServerTime 기준으로 만료된 시간 제한들 삭제.
	void ClearExpiredTimes( float InServerTime = -1.f );
	
	// 모든 제한시간 삭제
	void Reset();
private:
	// 측정 중인 시간들 모음. TPair<제한 시간, 제한 시간 체크의 기준이 되는 시작 시간(서버)>
	TMap<const Type, TPair<float, float>> TimeLimits;
};

#include "TimeLimitChecker.inl"
