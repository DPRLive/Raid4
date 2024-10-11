// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillBase.h"
#include "R4AnimSkillBase.generated.h"

class IR4NotifyByIdInterface;

/**
 * Skill Anim Update에 사용
 */
struct FR4AnimSkillExecuteInfo
{
	FR4AnimSkillExecuteInfo() = delete;

	FR4AnimSkillExecuteInfo ( int32 InSkillAnimKey, int32 InMontageInstanceID, float InExecuteServerTime, TFunction<void()>&& InFunc )
	: SkillAnimKey( InSkillAnimKey )
	, MontageInstanceID( InMontageInstanceID )
	, ExecuteServerTime( InExecuteServerTime )
	, Func ( MoveTemp(InFunc) )
	{}

	// Skill Anim Key
	int32 SkillAnimKey;

	// 해당 Execute를 등록 시, 해당 Anim에 할당 받은 Instance ID
	int32 MontageInstanceID;
	
	// 실행되어야 하는 Server Time
	float ExecuteServerTime;

	// 실행시킬 Func
	TFunction<void()> Func;
};

/**
 * Server에서 Play 중인 Animation의 정보
 */
USTRUCT()
struct FAnimPlayServerStateInfo
{
	GENERATED_BODY()
	
	FAnimPlayServerStateInfo()
	: AnimStartServerTime( -1.f )
	, SectionIndex( INDEX_NONE )
	, MontageInstanceID( INDEX_NONE )
	{}
	
	// 실행이 시작 된 시간, < 0 일 시 Play중이 아닌것으로 판단
	UPROPERTY( VisibleInstanceOnly, Transient )
	float AnimStartServerTime;

	// Section Index
	UPROPERTY( VisibleInstanceOnly, Transient )
	int32 SectionIndex;

	// Montage Instance ID. Anim이 중복 실행인지 판별하는데에 사용.
	UPROPERTY( VisibleInstanceOnly, Transient )
	int32 MontageInstanceID;
};

/**
 * Skill Anim을 사용하는 Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
 *
 * 같은 Anim을 두번 사용하면, Blend Out 시점을 End Play Anim으로 판정하더라도
 * Begin 2 () -> End 1 () 순서로 로직이 진행되어 문제가 생김. ( Montage Instance는 파괴되지만, Anim은 queue에서 대기 )
 * 그래서 Montage Instance 파괴 시점을 Get해서 해결해두었음
 * 
 * < 상속하여 클래스를 제작 시 주의할 점 >
 * ( FR4SkillAnimInfo 사용 시 )
 * - Skill Anim Key값을 Server로 부터 할당 받을 수 있도록 Replicate 설정. Key만 Replicate 하면 된다면, COND_InitialOnly 권장
 * - Skill Anim 키 값에 맞는 Valid Check 조건을 설정
 * (OnBeginSkillAnim(), OnEndSkillAnim(), PlaySkillAnim_Validate(), PlaySkillAnim_Ignore() 등 활용)
 * ( Skill 타이밍 체크 )
 * - Skill 사용을 적절히 체크하여 Skill CoolDownTime을 체크 및 설정 (SetSkillCoolDownTime())
 * - Skill 사용을 적절히 체크하여 시작 및 종료를 알려야 함! (OnBeginSkill, OnEndSkill)
 *  (Execute)
 * - PendingExecutes를 함부로 접근하여 지우지 말 것! Montage Instance 비교해서 알아서 지움!)
 */
UCLASS( Abstract, ClassGroup=(Skill) )
class RAID4_API UR4AnimSkillBase : public UR4SkillBase
{
	GENERATED_BODY()

public:
	UR4AnimSkillBase();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
protected:
	// Skill Anim Key에 맞는, 특정 시간 뒤에 Execute 예약 추가. Anim을 Play 시킨 후 넣을 것 !
	void AddExecute( int32 InSkillAnimKey, TFunction<void()>&& InFunc, float InDelay, float InDelayRate = 1.f );

	// Detect Execute 추가
	void AddDetectExecute( int32 InSkillAnimKey, const FR4SkillTimeDetectWrapper& InTimeDetectInfo, float InDelayRate = 1.f );

	// Buff Execute 추가
	void AddBuffExecute( int32 InSkillAnimKey, const FR4SkillTimeBuffWrapper& InTimeBuffInfo, float InDelayRate = 1.f );

	// Time의 Length와, 시작 시간이 주어졌을때, 현재 서버 타임과 비교하여 얼마나 빠르게 실행해야 서버와 동일하게 끝낼 수 있는지 delay 계산
	float CalculateDelayRate( float InTotalLength, float InStartTime );

	// Skill Animation을 Local에서 Play. InStartServerTime으로 동기화 가능.
	bool PlaySkillAnimSync_Local( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime );
	
	// Skill Animation을 Play 후 Server RPC 전송을 통해 서버와 동기화. 멤버로 등록된 Skill Anim만 Server에서 Play 가능.
	bool PlaySkillAnim_WithServerRPC( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName );
	
	// Anim을 Play시작 시 호출.
	virtual void OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime );
	
	// Anim 종료 시 호출.
	virtual void OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo );
	
	// Server RPC의 Validation check 시 사용
	virtual bool PlaySkillAnim_Validate( int32 InSkillAnimKey ) const;

	// Server RPC의 Play Skill Anim 시 요청 무시 check에 사용
	virtual bool PlaySkillAnim_Ignore( int32 InSkillAnimKey ) const;

	// Key에 맞는 Skill Anim이 Server에서 Play되고 있는지 확인 
	bool IsSkillAnimServerPlaying( int32 InSkillAnimKey ) const;

	// Skill Anim Key가 Server에서 할당 받은 Valid한 Key인지 확인
	FORCEINLINE bool IsValidSkillAnimKey( int32 InSkillAnimKey ) const { return AnimPlayServerStates.IsValidIndex( InSkillAnimKey ); }

	// Key에 맞는 멤버로 등록된 FR4SkillAnimInfo를 찾아서 return
	const FR4SkillAnimInfo* GetSkillAnimInfo( int32 InSkillAnimKey ) const;

	// 현재 Tick이 필요한 상태인지 return.
	virtual bool IsNeedTick() const;
	
private:
	// Execute를 Update
	void _UpdateExecute( float InNowServerTime );

	// InSkillAnimKey의 Montage를 현재 Play 시키고 있는 Montage Instance ID를 Get
	int32 _GetNowMontageInstanceID( int32 InSkillAnimKey ) const;
	
	// Server로 Skill Anim Play를 전송.
	UFUNCTION( Server, Reliable, WithValidation )
	void _ServerRPC_PlaySkillAnim( int32 InSkillAnimKey, int32 InStartSectionIndex, float InStartServerTime );

	// Skill Anim 멤버를 파싱
	void _ParseSkillAnimInfo();

	// Server에서 Play되는 Anim 상태가 변동 시 호출
	UFUNCTION()
	void _OnRep_AnimPlayServerState( const TArray<FAnimPlayServerStateInfo>& InPrevAnimPlayServerStates );
	
private:
	// 호출 대기중인 Executes ( Update Buffer, Pending Update Buffer )
	TArray<FR4AnimSkillExecuteInfo> PendingExecutes;
	
    // 서버에서의 해당 Skill Key의 Anim의 Play 상태.
	// Index(0 ~ N) : 현재 Skill에 할당 된 멤버 SkillAnimInfo들의 SkillAnimKey
	UPROPERTY( ReplicatedUsing = _OnRep_AnimPlayServerState, Transient )
	TArray<FAnimPlayServerStateInfo> AnimPlayServerStates;
	
	// parsing 하며 찾아낸 FR4SkillAnimInfo 멤버 변수들을 Caching
	// Server, Client 모두 Parsing하며, key는 server에서 부여되기에 일반 TArray로 캐싱만 해둠
	TArray<const FR4SkillAnimInfo*> CachedSkillAnimInfos;

	// ClearMontageInstance 이벤트를 수신하는 DelegateHandles 캐싱.
	TMap<int32, FDelegateHandle> CachedClearMontageHandles;
};