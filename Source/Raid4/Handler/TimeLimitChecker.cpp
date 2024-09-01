#include "TimeLimitChecker.h"

FTimeLimitChecker::FTimeLimitChecker()
{
	TimeLimitDuration = 0.f;
	TimeLimitStartServerTime = 0.f;
}

/**
 *  새로운 Time 제한을 적용.
 *  @param InTimeLimitDuration : 제한 시간
 *  @param InServerTime : 제한시간 체크의 기준이 되는 시작시간(서버).
 */
void FTimeLimitChecker::SetNewTimeCheck( float InTimeLimitDuration, float InServerTime )
{
	TimeLimitDuration = InTimeLimitDuration;
	TimeLimitStartServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);
}

/**
 *  남은 제한 시간을 리턴
 *  @param InServerTime : 현재 서버 시간.
 */
float FTimeLimitChecker::GetRemainingTime( float InServerTime ) const
{
	// 서버시간 안줬으면 R4GetServerTimeSeconds 사용
	InServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);
	float remainTime = TimeLimitStartServerTime + TimeLimitDuration - InServerTime;
	
	return FMath::Clamp(remainTime, 0.f, remainTime);
}

/**
 *  제한시간이 지났는지 리턴
 *  @param InServerTime : 현재 서버 시간.
 */
bool FTimeLimitChecker::IsTimeLimitExpired( float InServerTime ) const
{
	// 서버시간 안줬으면 R4GetServerTimeSeconds 사용
	InServerTime = (InServerTime < 0.f ? R4GetServerTimeSeconds() : InServerTime);

	// 남은 제한 시간이 0인지 확인
	return FMath::IsNearlyZero(GetRemainingTime(InServerTime));
}
