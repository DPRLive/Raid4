// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillBase.h"
#include "R4AnimSkillBase.generated.h"

class IR4NotifyDetectInterface;

/**
 * Skill Anim을 사용하는 Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
 * < 상속하여 클래스를 제작 시 주의할 점 >
 * ( FR4SkillAnimInfo 사용 시 )
 * - Skill Anim Key값을 Server로 부터 할당 받을 수 있도록 Replicate 설정
 * - Skill Anim 키 값에 맞는 Valid Check 조건을 설정 (OnBeginSkillAnim(), IsLockPlaySkillAnim())
 * ( FR4SkillDetectInfo 사용 시 )
 * - Detector Key값을 Server로 부터 할당 받을 수 있도록 Replicate 설정
 * ( Skill 타이밍 체크 )
 * - Skill 사용을 적절히 체크하여 Skill CoolDownTime을 체크 및 설정 (SetSkillCoolDownTime())
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

	virtual void BeginPlay() override;
	
protected:
	// Skill Animation을 Play. 멤버로 등록된 Skill Anim만 Server에서 Play 가능.
	bool PlaySkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo );

	// Anim을 Play시작 시 호출. Key에 맞춰 할 로직 설정. Server와 Owner Client 에서 호출.
	virtual void OnBeginSkillAnim( uint32 InSkillAnimKey ) {}
	
	// Skill Anim Key 값에 맞는 Skill Anim 을 현재 Play할 수 없는지 확인.
	// 기본적으로 PlayAnim Server RPC에서 Validation Check에 사용
	virtual bool IsLockPlaySkillAnim( uint32 InSkillAnimKey ) const { return false; }

private:
	// Server로 Skill Anim Play를 전송.
	UFUNCTION( Server, Reliable, WithValidation )
	void _ServerRPC_PlaySkillAnim( uint32 InSkillAnimKey );
	
	// Skill Anim 멤버를 찾아서 DetectNotify <-> FR4SkillDetectEffectInfo를 Bind. 및 Skill Anim 키 부여
	void _ParseSkillAnimInfo();

	// DetectNotify <-> FR4DetectEffectWrapper 연결
	void _BindDetectNotifyAndEffect( const TScriptInterface<IR4NotifyDetectInterface>& InDetectNotify, const FR4DetectEffectWrapper& InDetectEffectInfo );
	
private:
	// SkillAnimInfo를 Bind하며 찾은 현재 클래스 멤버에 있는 SkillAnimInfo의 개수, Skill Anim Key 부여용으로도 사용
	uint32 CachedSkillAnimInfoCount;

	// Key값이 어떤 SkillAnim을 가리키는지 확인하기 위한 CachedMap (Server Only)
	// { Skill Anim Key, UAnimMontage* }
	TMap<uint32, TWeakObjectPtr<UAnimMontage>> Server_CachedSkillAnimKey;
};
