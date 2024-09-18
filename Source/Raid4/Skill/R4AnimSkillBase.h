﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillBase.h"
#include "R4AnimSkillBase.generated.h"

class IR4NotifyByIdInterface;

/**
 * Skill Anim을 사용하는 Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
 * < 상속하여 클래스를 제작 시 주의할 점 >
 * ( FR4SkillAnimInfo 사용 시 )
 * - Skill Anim Key값을 Server로 부터 할당 받을 수 있도록 Replicate 설정
 * - Skill Anim 키 값에 맞는 Valid Check 조건을 설정 (OnBeginSkillAnim(), OnEndSkillAnim(), PlaySkillAnim_Validate(), PlaySkillAnim_Ignore() 등 활용)
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

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
protected:
	// Skill Animation을 Play. 멤버로 등록된 Skill Anim만 Server에서 Play 가능.
	bool PlaySkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo );
	
	// Anim을 Play시작 시 호출. Server와 Owner Client 에서 호출.
	virtual void OnBeginSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo );

	// Anim 종료 시 호출. Server와 Owner Client 에서 호출
	virtual void OnEndSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted );

	// Server RPC의 Validation check 시 사용
	virtual bool PlaySkillAnim_Validate( uint32 InSkillAnimKey ) const;

	// Server RPC의 Play Skill Anim 시 요청 무시 check에 사용
	virtual bool PlaySkillAnim_Ignore( uint32 InSkillAnimKey ) const;

	// Key에 맞는 Skill Anim이 Server에서 Play되고 있는지 확인 
	bool IsSkillAnimPlaying( uint32 InSkillAnimKey ) const;
private:
	// Server로 Skill Anim Play를 전송.
	UFUNCTION( Server, Reliable, WithValidation )
	void _ServerRPC_PlaySkillAnim( uint32 InSkillAnimKey, float InStartServerTime );
	
	// Server에서 Skill Anim 멤버를 찾아서 Skill Anim 키 부여
	void _Server_ParseSkillAnimInfo();

	// InMontageInstanceId를 Key로 DetectNotify <-> ExecuteDetect() 연결
	void _BindNotifiesAndDetect( int32 InMontageInstanceId, const FR4SkillAnimInfo& InSkillAnimInfo );

	// InMontageInstanceId를 Key로 Bind해 두었던 DetectNotify <-> ExecuteDetect() unbind
	void _UnbindNotifiesAndDetect( int32 InMontageInstanceId, const FR4SkillAnimInfo& InSkillAnimInfo );
	
private:
	// Skill Anim들을 Play할 수 있는지 확인에 체크
	// Index(1 ~ CachedSkillAnimInfoCount): SkillAnimKey, Value (true : 사용 중) 
	UPROPERTY( Replicated, Transient )
	TArray<bool> SkillAnimPlayState;
	
	// SkillAnimInfo를 Bind하며 찾은 현재 클래스 멤버에 있는 SkillAnimInfo의 개수, Skill Anim Key 부여용으로도 사용
	uint32 Server_CachedSkillAnimInfoCount;

	// Skill Anim Key값이 어떤 SkillAnimInfo를 가리키는지 확인하기 위한 CachedMap (Server Only)
	// { Skill Anim Key, UAnimMontage* }
	TMap<uint32, const FR4SkillAnimInfo*> Server_CachedSkillAnimInfo;
};
