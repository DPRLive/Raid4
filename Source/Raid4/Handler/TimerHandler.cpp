#include "TimerHandler.h"

/**
 * 생성자
 */
FTimerHandler::FTimerHandler(UObject* InUObject)
{
	Handle = FTimerHandle();
	Owner = InUObject;
	CachedFunction = TFunction<void()>();
	CachedDuration = 0.f;
}

/**
 * 소멸자
 */
FTimerHandler::~FTimerHandler()
{
    // 소멸 시 clear.
    ClearTimer();
}

/**
 * 실행 할 함수 제거
 */
void FTimerHandler::ResetFunction()
{
	CachedFunction.Reset();
}

/**
 * 기본 타이머 적용.
 * @param InDuration : 사용할 시간. 0.f >= InTime이면 clear됨
 */
void FTimerHandler::SetTimer(float InDuration)
{
	if(!Owner.IsValid())
		return;

	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world))
		return;
	
	CachedDuration = InDuration;
	
	world->GetTimerManager().SetTimer(Handle, [thisPtr = TWeakPtr<FTimerHandler>(AsWeak())]
	{
		if(!thisPtr.IsValid())
			return;

		TSharedPtr<FTimerHandler> thisHandler = thisPtr.Pin();

		if(thisHandler->CachedFunction)
			thisHandler->CachedFunction();

		thisHandler->OnCompletedTimerDelegate.ExecuteIfBound();
	}, InDuration, false);
}

/**
 * loop timer 적용
 * @param InInterval : 간격. 0.f >= InTime이면 clear됨
 */
void FTimerHandler::SetLoopTimer(float InInterval)
{
	if(!Owner.IsValid())
		return;

	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world))
		return;

	CachedDuration = -1.f;
	
	world->GetTimerManager().SetTimer(Handle, [thisPtr = TWeakPtr<FTimerHandler>(AsWeak())]
	{
		if(!thisPtr.IsValid())
			return;

		TSharedPtr<FTimerHandler> thisHandler = thisPtr.Pin();
		
		if(thisHandler->CachedFunction)
			thisHandler->CachedFunction();
		
	}, InInterval, true);
}

/**
 * 	loop duration timer 적용 ( loop인데 시간이 정해져 있음 )
 * @param InInterval : 간격. 0.f >= InTime이면 clear됨
 * @param InDuration : 몇초 동안 진행 할 것인지
 */
void FTimerHandler::SetLoopDurationTimer(float InInterval, float InDuration)
{
	if(!Owner.IsValid())
		return;

	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world))
		return;
		
	CachedDuration = InDuration;
	
	world->GetTimerManager().SetTimer(Handle,[thisPtr = TWeakPtr<FTimerHandler>(AsWeak())]
	{
		if(!thisPtr.IsValid())
			return;

		TSharedPtr<FTimerHandler> thisHandler = thisPtr.Pin();

		// 타이머 걸고 난 뒤 지난 시간과 비교
		if(thisHandler->GetElapsedTime() >= thisHandler->CachedDuration)
		{
			thisHandler->ClearTimer();
			return;
		}

		if(thisHandler->CachedFunction)
			thisHandler->CachedFunction();
		
	}, InInterval, true);
}

/**
 * 타이머를 해제한다.
 */
void FTimerHandler::ClearTimer()
{
	if(!Owner.IsValid())
		return;

	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world) || !Handle.IsValid())
		return;

	world->GetTimerManager().ClearTimer(Handle);

	OnCompletedTimerDelegate.ExecuteIfBound();
}

/**
 * 타이머가 걸려 있는지 확인
 */
bool FTimerHandler::IsActiveTimer() const
{
	if(!Owner.IsValid())
		return false;

	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world))
		return false;

	return world->GetTimerManager().IsTimerActive(Handle);
}

/**
 * 남은 시간을 리턴
 * @return : 남은 시간을 반환. loop timer 시 -1.f, 걸려 있지 않은 경우 0.f
 */
float FTimerHandler::GetRemainingTime() const
{
	// 루프일 시
	if(FMath::IsNearlyEqual(-1.f, CachedDuration))
		return -1.f;

	// 안걸려 있을 시
	if(!Handle.IsValid())
		return 0.f;

	// 총 시간 - 경과한 시간을 반환
	return FMath::Clamp(CachedDuration - GetElapsedTime(), 0.f, CachedDuration);
}

/**
 * 타이머를 걸고 난 뒤 지난 시간을 리턴
 * @return : 타이머를 걸고 난 뒤 지난 시간, 걸려 있지 않은 경우 0.f를 return
 */
float FTimerHandler::GetElapsedTime() const
{
	if(!Owner.IsValid())
		return 0.f;
	
	UWorld* world = R4GetWorld(Owner.Get());
	if(!IsValid(world) || !Handle.IsValid())
		return 0.f;

	return world->GetTimerManager().GetTimerElapsed(Handle);
}
