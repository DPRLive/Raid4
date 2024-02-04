#include "CoolTimeHandler.h"

#include <TimerManager.h>

/**
 * 생성자
 */
FCoolTimeHandler::FCoolTimeHandler()
{
   Handle = FTimerHandle();
}

/**
 * 소멸자
 */
FCoolTimeHandler::~FCoolTimeHandler()
{
    // 람다 내부에서 this를 캡처해갔으므로 위험할 수 있으니 소멸 시 clear.
    ClearCoolTime();
}

/**
 * 쿨타임을 적용한다.
 * @param InCoolTime : 사용할 쿨타임. 0.f >= InCoolTime이면 clear됨
 */
void FCoolTimeHandler::SetCoolTime(const float InCoolTime)
{
	R4GetWorld()->GetTimerManager().SetTimer(Handle, [this]
	{
	    if(OnCompletedCoolTime.IsBound())
	        OnCompletedCoolTime.Broadcast();
	}, InCoolTime, false);
}

/**
 * 쿨타임을 해제한다.
 */
void FCoolTimeHandler::ClearCoolTime()
{
    R4GetWorld()->GetTimerManager().ClearTimer(Handle);
}

/**
 * 남은 쿨타임을 가져온다.
 * @return : 쿨타임을 반환, 쿨타임이 걸려있지 않은경우 0.f를 반환
 */
const float FCoolTimeHandler::GetCoolTime() const
{
    if(R4GetWorld()->GetTimerManager().IsTimerActive(Handle))
        return R4GetWorld()->GetTimerManager().GetTimerRemaining(Handle);

    return 0.f;
}

/**
 * 쿨타임을 감소시킨다. 감소 후 쿨타임이 0보다 작거나 같으면 완료된걸로 간주.
 * @param InReduceTime : 감소시키고 싶은 시간
 * @return : 감소된 후 쿨타임. 남은 쿨타임보다 감소량이 더 많으면 0.f 반환 
 */
float FCoolTimeHandler::ReduceCoolTime(const float InReduceTime)
{
    float newCoolTime = FMath::Clamp(GetCoolTime() - InReduceTime, 0.f, GetCoolTime() - InReduceTime);

	// 감소했는데 0이면 완료된거로 간주
	if(FMath::IsNearlyZero(newCoolTime))
	{
		ClearCoolTime();

		if(OnCompletedCoolTime.IsBound())
			OnCompletedCoolTime.Broadcast();
		
		return newCoolTime;
	}

	SetCoolTime(newCoolTime);
	return newCoolTime;
}
