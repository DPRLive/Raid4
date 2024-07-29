#pragma once

#include <TimerManager.h>

// Callable 객체인지 확인
template<typename T>
concept CCallableType = std::is_invocable_v<T>;

/**
 * Timer를 좀 더 편하게 쓰기 위한 Handler.
 * 한개의 FTimerHandler로 여러개의 timer를 걸 순 없음
 * BeginDestroy시 Clear하면 좋을듯
 * TSharedPtr과 함께 사용
 */
class FTimerHandler : public TSharedFromThis<FTimerHandler>
{
public:
	FTimerHandler() = delete;
	
	// 생성자
	FTimerHandler(UObject* InUObject = nullptr);

	// 소멸자
	virtual ~FTimerHandler();

	// 실행 할 함수를 설정
	template<CCallableType T>
	void SetFunction(T&& InFunc);

	// 실행 할 함수 제거
	void ResetFunction();
	
	// Function이 세팅 되어있는지 여부 리턴
	FORCEINLINE bool IsSetFunc() const { return CachedFunction.operator bool(); }
	
	// 기본 타이머 적용.
	void SetTimer(float InDuration);

	// loop timer 적용
	void SetLoopTimer(float InInterval);

	// loop duration timer 적용 ( loop인데 시간이 정해져 있음 )
	void SetLoopDurationTimer(float InInterval, float InDuration);
	
	// 타이머를 해제한다.
	void ClearTimer();

	// 타이머가 걸려 있는지 확인
	bool IsActiveTimer() const;
	
	// 남은 시간을 리턴
	float GetRemainingTime() const;

	// 타이머를 걸고 난 뒤 지난 시간을 리턴
	float GetElapsedTime() const;

	// Timer가 종료 (clear 또는 끝까지 갈) 시 broadcast할 delegate
	FSimpleDelegate OnCompletedTimerDelegate;
	
private:
	// Timer를 사용하기 위한 Handle
    FTimerHandle Handle;

	// 이걸 사용하는 Owner
	TWeakObjectPtr<UObject> Owner;

	// Cached
	// 등록된 Lambda
	TFunction<void(void)> CachedFunction;
	
	// Duration을 캐싱. -1.f : loop
	float CachedDuration;
};

/**
 * 함수를 설정
 */
template <CCallableType T>
void FTimerHandler::SetFunction(T&& InFunc)
{
	CachedFunction = Forward<T>(InFunc);
}
