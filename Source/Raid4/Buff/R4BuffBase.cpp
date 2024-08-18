// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BuffBase.h"
#include "R4BuffDesc.h"
#include "Calculator/R4BuffValueCalculatorInterface.h"
#include "../Handler/TimerHandler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffBase)

UR4BuffBase::UR4BuffBase()
{
	BuffDesc = FR4BuffDesc();
	bDeactivate = false;
}

/**
 *  Pool에 반납 전 로직 처리
 */
void UR4BuffBase::PreReturnPoolObject()
{
	Clear();
}

/**
 *  버프를 적용
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용. 클래스마다 다르게 적용 될 수 있음.
 *  @return : 버프 적용 성공 실패 여부
 */
bool UR4BuffBase::ApplyBuff(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	// 준비 실패 시 버프를 걸지 않고 false 리턴.
	bool bReady = PreActivate(InInstigator, InVictim, InBuffDesc);
	if(!bReady)
	{
		LOG_WARN(R4Log, TEXT("Failed to Apply Buff."));
		return false;
	}
	
	// Buff Desc의 정보에 따라서 버프를 실행
	_ActivateByBuffMode(BuffDesc.BuffMode);
	_SetBuffRemoveTiming(BuffDesc.BuffDurationType);

	return true;
}

/**
 *  버프를 제거, 제거 로직 실행 후 버프가 끝남을 알리고 버프를 Clear.
 *  ex) 제거로직 : duration 실행의 경우, 실행 시간이 지나면 원래대로 되돌려야함.
 */
void UR4BuffBase::RemoveBuff()
{
	// 복구 로직이 필요한 경우
	if(bDeactivate)
		Deactivate();
	
	// 버프가 끝남을 알림.
	if (OnEndBuffDelegate.IsBound())
		OnEndBuffDelegate.Broadcast();

	// 버프를 Clear.
	Clear();
}

/**
 *  버프가 세팅해야 하는 것을 정의.
 *  클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용. 클래스마다 다르게 적용 될 수 있음.
 *  @return : 세팅 성공 실패 여부
 */
bool UR4BuffBase::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	CachedInstigator = InInstigator;
	CachedVictim = InVictim;
	
	if (InBuffDesc != nullptr)
		BuffDesc = *InBuffDesc;
	
	// 버프의 시전자 / 받은자가 유효한지
	return (CachedInstigator.IsValid() && CachedVictim.IsValid());
}

/**
 *  버프가 실제로 할 로직을 정의
 */
void UR4BuffBase::Activate()
{
	// BuffDesc의 Value 값을 계산
	_CalculateBuffValue();
}

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4BuffBase::Clear()
{
	CachedInstigator.Reset();
	CachedVictim.Reset();
	
	// Timer 정리
	if(TimerHandler.IsValid())
		TimerHandler->Reset();
	
	// CDO대로 BuffDesc 되돌리기
	UR4BuffBase* cdo = GetClass() ? GetClass()->GetDefaultObject<UR4BuffBase>() : nullptr;
	if(IsValid(cdo))
		BuffDesc = cdo->BuffDesc;

	// OnEndBuffDelegate Clear
	OnEndBuffDelegate.Clear();
}

/**
 *  EBuffValueType에 따라서 BuffDesc의 Value를 계산
 */
void UR4BuffBase::_CalculateBuffValue()
{
	if(BuffDesc.BuffValueType == EBuffValueType::CustomClass)
	{
		if(!IsValid(BuffDesc.BuffValueCalculatorClass) ||
			!ensureMsgf(BuffDesc.BuffValueCalculatorClass->ImplementsInterface(UR4BuffValueCalculatorInterface::StaticClass()),
			TEXT("BuffValue Calculator class must implement IR4BuffValueCalculatorInterface.")))
			return;
	
		// CDO를 사용하여 Buff의 Value를 산출
		const UObject* cdo = BuffDesc.BuffValueCalculatorClass->GetDefaultObject(true);
		if(const IR4BuffValueCalculatorInterface* buffValueCalculator = Cast<IR4BuffValueCalculatorInterface>(cdo))
			BuffDesc.Value = buffValueCalculator->CalculateBuffValue(CachedInstigator.Get(), CachedVictim.Get());
	}
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

	const TSharedPtr<FTimerHandler>& timerHandler = _GetTimerHandler();
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
const TSharedPtr<FTimerHandler>& UR4BuffBase::_GetTimerHandler()
{
	if(TimerHandler.IsValid())
		return TimerHandler;

	return TimerHandler = MakeShared<FTimerHandler>(this);
}