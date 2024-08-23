// Fill out your copyright notice in the Description page of Project Settings.

#include "R4BuffManageComponent.h"
#include "R4BuffBase.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffManageComponent)

UR4BuffManageComponent::UR4BuffManageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 적당히 갱신시간 타협
	SetComponentTickInterval(Buff::G_BuffTickInterval);
	
	SetIsReplicatedByDefault(true);
}

void UR4BuffManageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4BuffManageComponent, UpdatingBuffs, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4BuffManageComponent, NonUpdatingBuffs, COND_OwnerOnly);
}

void UR4BuffManageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UR4BuffManageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 버프 업데이트
	if(GetOwnerRole() == ROLE_Authority)
		_Server_UpdateBuffs();
}

/**
 *	버프를 추가 (서버)
 *	@param InInstigator : 버프를 부여한 액터
 *	@param InBuffClass : 부여할 버프 클래스
 *	@param InBuffSettingDesc : 버프 사용에 관한 세팅 값 
 */
void UR4BuffManageComponent::Server_AddBuff(AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	FBuffAppliedInfo buffInfo;
	buffInfo.ServerBuffInstance = Cast<UR4BuffBase>(OBJECT_POOL->GetObject(InBuffClass));

	// 버프 인스턴스가 유효하지 않으면 리턴
	if(!IsValid(buffInfo.ServerBuffInstance)) 
		return;
	
	// 버프 준비, 적용 실패 시 Object Pool에 반납
	bool bSetupSuccess = buffInfo.ServerBuffInstance->SetupBuff(InInstigator, GetOwner());
	if(!bSetupSuccess)
	{
		OBJECT_POOL->ReturnPoolObject(buffInfo.ServerBuffInstance);
		return;
	}
	
	// 추가 정보 설정
	buffInfo.BuffClass = InBuffClass;
	buffInfo.BuffSettingDesc = InBuffSettingDesc;
	
	// 버프 등록
	_Server_RegisterBuffBySetting(MoveTemp(buffInfo));
}

/**
 *	태그로 해당하는 버프를 모두 제거
 *	(BuffTag.MatchFunc(InTagToQuery))
 *	@param InTagToQuery : 쿼리에 사용할 버프 식별 태그
 *	@param InQueryType : 쿼리 타입
 */
void UR4BuffManageComponent::Server_RemoveBuffAllByTag(const FGameplayTag& InTagToQuery, EGameplayTagQueryType InQueryType)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	auto tagQuery = [&InTagToQuery, InQueryType](const FBuffAppliedInfo& InBuffInfo)
	{
		// InTagToQuery가 부모로 사용된 경우도 포함
		if(InQueryType == EGameplayTagQueryType::Match)
			return InBuffInfo.ServerBuffInstance->GetBuffTag().MatchesTag(InTagToQuery);

		// 완전히 일치
		if(InQueryType == EGameplayTagQueryType::MatchExact)
			return InBuffInfo.ServerBuffInstance->GetBuffTag().MatchesTagExact(InTagToQuery);

		return false;
	};

	_Server_BuffArrayRemoveAllByPredicate(UpdatingBuffs, tagQuery);
	_Server_BuffArrayRemoveAllByPredicate(NonUpdatingBuffs, tagQuery);
}

/**
 *	태그로 해당하는 버프를 모두 제거
 *	(BuffTag.MatchFunc(InTagContainerToQuery))
 *	@param InTagContainerToQuery : 쿼리에 사용할 버프의 식별 태그들
 *	@param InQueryType : 쿼리 타입
 */
void UR4BuffManageComponent::Server_RemoveBuffAllByTagContainer(const FGameplayTagContainer& InTagContainerToQuery, EGameplayTagQueryType InQueryType)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	auto tagQuery = [&InTagContainerToQuery, InQueryType](const FBuffAppliedInfo& InBuffInfo)
	{
		// InTagToQuery가 부모로 사용된 경우도 포함
		if(InQueryType == EGameplayTagQueryType::Match)
			return InBuffInfo.ServerBuffInstance->GetBuffTag().MatchesAny(InTagContainerToQuery);

		// 완전히 일치
		if(InQueryType == EGameplayTagQueryType::MatchExact)
			return InBuffInfo.ServerBuffInstance->GetBuffTag().MatchesAnyExact(InTagContainerToQuery);

		return false;
	};

	_Server_BuffArrayRemoveAllByPredicate(UpdatingBuffs, tagQuery);
	_Server_BuffArrayRemoveAllByPredicate(NonUpdatingBuffs, tagQuery);
}

/**
 *	버프 세팅에 맞춰 버프를 등록
 *	@param InBuffAppliedInfo : 적용 버프의 정보.
 */
void UR4BuffManageComponent::_Server_RegisterBuffBySetting(FBuffAppliedInfo&& InBuffAppliedInfo)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	// 버프 처음 사용
	InBuffAppliedInfo.ServerBuffInstance->ApplyBuff();
	InBuffAppliedInfo.FirstAppliedServerTime = InBuffAppliedInfo.LastAppliedServerTime = R4GetServerTimeSeconds(GetWorld());
	
	// 일회성 버프일 경우 등록 안함
	if(InBuffAppliedInfo.BuffSettingDesc.BuffDurationType == EBuffDurationType::OneShot)
		return;

	// 일정 기간마다 적용되는 버프이거나, 지속시간이 존재하는 버프라면 업데이트가 필요
	if(InBuffAppliedInfo.BuffSettingDesc.BuffDurationType == EBuffDurationType::Duration ||
		InBuffAppliedInfo.BuffSettingDesc.BuffMode == EBuffMode::Interval)
	{
		UpdatingBuffs.Emplace(MoveTemp(InBuffAppliedInfo));
		return;
	}

	// 아닌 경우 업데이트가 필요 없는 버프로 분류
	NonUpdatingBuffs.Emplace(MoveTemp(InBuffAppliedInfo));
}

/**
 *	버프를 업데이트. (Update Buff만 업데이트 함)
 */
void UR4BuffManageComponent::_Server_UpdateBuffs()
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;

	_Server_BuffArrayRemoveAllByPredicate(UpdatingBuffs,
	[nowServerTime = R4GetServerTimeSeconds(GetWorld())](FBuffAppliedInfo& InBuffInfo)
	{
		// 버프 갱신 //
		
		// 버프가 끝나야 하는 시간
		double buffEndTime = InBuffInfo.FirstAppliedServerTime + InBuffInfo.BuffSettingDesc.Duration;

		// 무한대의 버프일 경우, 최대로 설정
		if(InBuffInfo.BuffSettingDesc.BuffDurationType == EBuffDurationType::Infinite)
			buffEndTime = DBL_MAX;
		
		// interval 적용 시 마지막 Activation 시간으로 부터 interval이 지났다면 적용
		if(InBuffInfo.BuffSettingDesc.BuffMode == EBuffMode::Interval)
		{
			// last activation 시간으로부터 지난만큼 / interval이 되었어야 하는 만큼 적용
			// 끝나야 하는 시간보다 더 흐른 경우 추가 적용 방지를 위한 Min으로 Clamp
			// KINDA_SMALL_NUMBER를 빼주어 종료 시간과의 차이가 정확히 나누어 떨어지는 경우, 0초 시점에 한번 적용하였으니 추가 적용 방지 
			double passTime = FMath::Min(nowServerTime, buffEndTime - KINDA_SMALL_NUMBER) - InBuffInfo.LastAppliedServerTime;

			// 지난 시간 / interval 로 count 계산, Division by zero 방지
			int32 count = static_cast<int32>(passTime / FMath::Max(InBuffInfo.BuffSettingDesc.IntervalTime, KINDA_SMALL_NUMBER));
			
			for(int32 i = 0; i < count; i++)
			{
				InBuffInfo.ServerBuffInstance->ApplyBuff();
				InBuffInfo.LastAppliedServerTime += InBuffInfo.BuffSettingDesc.IntervalTime; // last activation time 갱신
			}
		}
		
		// 지속 시간이 지났다면 제거.
		if( FMath::IsNearlyEqual(buffEndTime, nowServerTime) || buffEndTime < nowServerTime )
			return true;

		return false;
	});
}

/**
 *	Buff Array에 대한 Remove All By Predicate, 인스턴스가 유효하지 않으면 자동으로 제거
 *	@param InBuffArray : 로직을 실행할 버프 배열
 *	@param InPredicate : Predicate
 */
void UR4BuffManageComponent::_Server_BuffArrayRemoveAllByPredicate(TArray<FBuffAppliedInfo>& InBuffArray, const TFunction<bool(FBuffAppliedInfo&)>& InPredicate)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	for(auto it = InBuffArray.CreateIterator(); it; ++it)
	{
		// 버프 인스턴스가 유효하지 않은 경우 -> Predicate이고 뭐고 바로 제거
		if(!IsValid(it->ServerBuffInstance))
		{
			it.RemoveCurrentSwap();
			continue;
		}

		// Predicate 만족 시
		if(InPredicate(*it))
		{
			// 제거 로직 실행
			it->ServerBuffInstance->RemoveBuff();

			// 인스턴스 Pool에 반납
			OBJECT_POOL->ReturnPoolObject(it->ServerBuffInstance);

			// 관리 목록에서 제거.
			it.RemoveCurrentSwap();
		}
	}
}
