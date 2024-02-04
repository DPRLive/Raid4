#pragma once

/**
 * 객체가 쿨타임을 걸 수 있게 하는 클래스
 * BeginDestroy시 Clear하면 좋을듯
 */
class FCoolTimeHandler
{
public:
	// 생성자
	FCoolTimeHandler();

	// 소멸자
	virtual ~FCoolTimeHandler();
	
	// 쿨타임을 적용한다.. 중복으로 쿨타임을 걸 수는 없음
	void SetCoolTime(const float InCoolTime);

	// 쿨타임을 해제한다.
	void ClearCoolTime();

	// 남은 쿨타임을 가져온다.
	const float GetCoolTime() const;

	// 쿨타임을 감소시킨다. 감소 후 쿨타임이 0보다 작거나 같으면 완료된걸로 간주.
	float ReduceCoolTime(const float InReduceTime);
	
	// 쿨타임이 완료되면 broadcast할 delegate
	DECLARE_MULTICAST_DELEGATE( FCompletedCoolTime )
	FCompletedCoolTime OnCompletedCoolTime;
	
private:
	// Timer를 사용하기 위한 Handle
    FTimerHandle Handle;
};
