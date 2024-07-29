// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BuffBase.h"
#include "R4BuffDesc.h"

#include "../Handler/TimerHandler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffBase)

UR4BuffBase::UR4BuffBase()
{
	BuffDesc = FR4BuffDesc();
	bDeactivate = false;
}

/**
 *  버프를 적용
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용. 클래스마다 다르게 적용 될 수 있음. 
 */
void UR4BuffBase::ApplyBuff(AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	// 버프를 준비
	PreActivate(InVictim, InBuffDesc);

	// Buff Desc의 정보에 따라서 버프를 실행
	_ActivateByBuffMode(BuffDesc.BuffMode);
	_SetBuffRemoveTiming(BuffDesc.BuffDurationType);
}

/**
 *  버프를 제거. 오버라이드 시 필요한거 하고 마지막에 Super를 호출.
 *  ex) duration 실행의 경우, 실행 시간이 지나면 원래대로 되돌려야함.
 */
void UR4BuffBase::RemoveBuff()
{
	if(TSharedPtr<FTimerHandler> timerHandler = _GetTimerHandler())
	{
		timerHandler->OnCompletedTimerDelegate.Unbind();
		timerHandler->ClearTimer();	
	}
	
	// 복구 로직이 필요한 경우
	if(bDeactivate)
		Deactivate();
	
	// 버프가 끝남을 알림.
	if (OnEndBuffDelegate.IsBound())
		OnEndBuffDelegate.Broadcast();
}

/**
 *  버프가 세팅해야 하는 것을 정의.
 *  클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용. 클래스마다 다르게 적용 될 수 있음. 
 */
void UR4BuffBase::PreActivate(AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	if (InBuffDesc != nullptr)
		BuffDesc = *InBuffDesc;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 */
void UR4BuffBase::Deactivate()
{
	PreDeactivate();
}

/**
 *  EBuffMode에 따라서 버프를 실행
 */
void UR4BuffBase::_ActivateByBuffMode(EBuffMode InBuffMode)
{
	// 일단 한번 적용 하고
	Activate();

	TSharedPtr<FTimerHandler> timerHandler = _GetTimerHandler();
	if(!timerHandler.IsValid())
		return;
	
	// Interval 시
	if(InBuffMode == EBuffMode::Interval)
	{
		// Timer에서 실행할 함수 지정
		timerHandler->SetFunction([thisPtr = TWeakObjectPtr<UR4BuffBase>(this)]
		{
			if(thisPtr.IsValid())
				thisPtr->Activate();
		});
	}
	else
	{
		timerHandler->ResetFunction();
	}
}

/**
 *  EBuffDurationType에 따라 버프의 제거 타이밍을 설정
 */
void UR4BuffBase::_SetBuffRemoveTiming(EBuffDurationType InDurationType)
{
	// 일회성 버프일 때
	if(InDurationType == EBuffDurationType::OneShot)
	{
		RemoveBuff();
		return;
	}

	TSharedPtr<FTimerHandler> timerHandler = _GetTimerHandler();
	if(!timerHandler.IsValid())
		return;
	
	// duration일 경우
	if(InDurationType == EBuffDurationType::Duration)
	{
		timerHandler->OnCompletedTimerDelegate.BindWeakLambda(this, [this]
		{
			RemoveBuff();
		});
		
		// 주기적으로 호출해야 하는 것이 있으면 Loop Duration Timer 설정
		if(timerHandler->IsSetFunc())
			timerHandler->SetLoopDurationTimer(BuffDesc.IntervalTime, BuffDesc.Duration);
		else // 아니면 그냥 타이머
			timerHandler->SetTimer(BuffDesc.Duration);
		
		return;
	}

	// Infinite 이며 호출해야 할게 있으면 loop 타이머를 설정
	if(InDurationType == EBuffDurationType::Infinite && timerHandler->IsSetFunc())
	{
		timerHandler->SetLoopTimer(BuffDesc.IntervalTime);
	}
}

/**
 *  Timer getter, 필요한 경우 생성 시킴
 */
TSharedPtr<FTimerHandler> UR4BuffBase::_GetTimerHandler()
{
	if(TimerHandler.IsValid())
		return TimerHandler;

	return TimerHandler = MakeShared<FTimerHandler>(this);
}