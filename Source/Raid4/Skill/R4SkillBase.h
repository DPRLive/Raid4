// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillStruct.h"

#include <Components/ActorComponent.h>

#include "R4SkillBase.generated.h"

struct FR4DetectResult;

/**
 * Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
 * < 상속하여 클래스를 제작 시 주의할 점 >
 * ( FR4SkillDetectInfo 사용 시 )
 * - Detector Key값을 Server로 부터 할당 받을 수 있도록 Replicate 설정
 * ( Skill 타이밍 체크 )
 * - Skill 사용을 적절히 체크하여 Skill CoolDownTime을 체크 및 설정 (SetSkillCoolDownTime())
 */
UCLASS( Abstract, HideCategories = (ComponentTick, Tags, Replication, ComponentReplication, Activation, Variable, Navigation, AssetUserData) )
class RAID4_API UR4SkillBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillBase();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// 스킬 사용이 가능한지 판단
	virtual bool CanActivateSkill() const;

	// 현재 남은 Skill CoolDown Time을 반환.
	float GetSkillCooldownRemaining() const;
	
protected:
	// Skill CoolDown Time을 적용. 
	void SetSkillCoolDownTime( float InCoolDownTime );

	// 현재 시점의 Skill의 CoolDownTime을 계산.
	float GetSkillCoolDownTime( bool InIsIgnoreReduction ) const;

	// 특정한 버프들을 적용.
	void Server_ApplyBuffs( AActor* InVictim, const TArray<FR4SkillBuffInfo>& InSkillBuffInfos ) const;

	// 특정한 데미지들을 적용.
	void Server_ApplyDamages( AActor* InVictim , const TArray<FR4SkillDamageInfo>& InSkillDamageInfos ) const;
	
	// Detect 실행
	void ExecuteDetect( const FR4DetectEffectWrapper& InDetectEffectInfo );

private:
	// FR4SkillDetectInfo 멤버를 찾아서 키를 부여.
	void _Server_ParseSkillDetectInfo();
	
	// Dummy Detector 생성
	void _CreateDummyDetector( const FR4SkillDetectInfo& InSkillDetectInfo );
	
	// Authority Detector 생성
	void _Server_CreateAuthorityDetector( const FR4DetectEffectWrapper& InDetectEffectInfo );

	// Server -> Owner Client로 Dummy 제거 요청.
	UFUNCTION( Client, Unreliable )
	void _ClientRPC_RemoveDummy( uint32 InDetectorKey );

private:
	// 해당 스킬의 쿨타임. 몇초마다 사용이 가능한지?
	UPROPERTY( EditAnywhere, meta = (UIMin = 0.f, ClampMin = 0.f, AllowPrivateAccess = true) )
	float BaseCoolDownTime;

	// 해당 스킬을 다시 사용할 수 있는 시간
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly )
	float CachedNextActivationServerTime;

	// Key를 부여하며 찾은 현재 클래스 멤버에 있는 FR4SkillDetectInfo의 개수, Detector Key 부여용으로도 사용
	uint32 CachedSkillDetectInfoCount;
	
	// Detector의 Dummy 체크를 위한 TMap ( Client )
	// { Detector Key, Detector Ptr }
	TMap<uint32, TWeakObjectPtr<UObject>> Client_CachedDetectorDummy;

	// Detector의 Server Detector 생성 체크를 위한 TMap ( Client )
	// { Detector Key }
	TSet<uint32> Client_CachedServerDetector;
};