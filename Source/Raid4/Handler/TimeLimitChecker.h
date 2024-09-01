#pragma once

/**
 * 특정 시간을 기록 후
 * 현재 시간과 비교할 때 사용하기 좋은 FTimeLimitChecker.
 * Tick을 굳이 사용해야 할 필요가 없을 때 사용하기 아주 좋지만
 * Tick을 사용하지 않기에 delegate가 없음
 */
class FTimeLimitChecker
{
public:
	FTimeLimitChecker();
	
	// 제한 시간을 리턴
	FORCEINLINE float GetTimeLimit() const { return TimeLimitDuration; }
	
	// 제한 시간 설정
	FORCEINLINE void SetTimeLimit( float InTimeLimitDuration ) { TimeLimitDuration = InTimeLimitDuration; }
	
	// 새로운 Time 제한 적용.
	void SetNewTimeCheck( float InTimeLimitDuration, float InServerTime = -1.f );

	// 남은 제한 시간을 리턴
	float GetRemainingTime( float InServerTime = -1.f ) const;

	// 제한시간이 지났는지 리턴
	bool IsTimeLimitExpired( float InServerTime = -1.f ) const;
	
private:
	// 제한 시간
	float TimeLimitDuration;
	
	// 제한시간 체크의 기준이 되는 시작시간(서버).
    float TimeLimitStartServerTime;
};
