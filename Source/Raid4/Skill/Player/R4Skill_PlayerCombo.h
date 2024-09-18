// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Input/R4PlayerInputableInterface.h"

#include "../R4AnimSkillBase.h"

#include "R4Skill_PlayerCombo.generated.h"

/**
 * Combo Skill Info
 */
USTRUCT( BlueprintType )
struct FR4ComboInputInfo
{
	GENERATED_BODY()

	// input test Notify Index
	UPROPERTY( VisibleAnywhere )
	uint8 NotifyNumber;
	
	// Notify가 현재 포함되어 있는 Section의 Index
    UPROPERTY( VisibleAnywhere )
    int32 NowSectionIndex;
	
	// 현재 Section의 Name
	UPROPERTY( VisibleAnywhere )
	FName NowSectionName;
	
	// 해당 Section의 Input 조건 만족 시 넘어갈 다음 Section을 설정
	UPROPERTY( EditAnywhere )
	FName NextSectionName;
};

/**
 * Player Combo Skill
 * ( Section 별로 Combo Input Test Notify 시,
 * Notify 전 입력이 진행되었으면 NextSection으로 Transition.
 * 아닌 경우 재생하던 Section을 끝까지 재생.)
 */
UCLASS( Blueprintable, NotBlueprintType, ClassGroup=(Skill) )
class RAID4_API UR4Skill_PlayerCombo : public UR4AnimSkillBase, public IR4PlayerInputableInterface
{
	GENERATED_BODY()

public:
	UR4Skill_PlayerCombo();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// ~ Begin IR4PlayerInputableInterface
	virtual void OnInputStarted() override;
	virtual void OnInputTriggered() override {}
	virtual void OnInputCompleted() override {}
	// ~ End IR4PlayerInputableInterface

protected:
	// Anim을 Play시작 시 호출. Server와 Owner Client 에서 호출.
	virtual void OnBeginSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo ) override;

	// Anim 종료 시 호출. Server와 Owner Client 에서 호출
	virtual void OnEndSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted ) override;

	// Server RPC의 Play Skill Anim 시 요청 무시 check에 사용
	virtual bool PlaySkillAnim_Ignore( uint32 InSkillAnimKey ) const override;
private:
	// Combo Input을 서버에 요청
	UFUNCTION( Server, Reliable )
	void _ServerRPC_RequestComboInput();
	
	// Combo Input Test
	void _ComboInputTest( uint8 InNotifyNumber );
	
	// Bind Combo Input Test Notify
	void _BindNotifiesAndInputTest( int32 InMontageInstanceId );

	// Bind Combo Input Test Notify
	void _UnbindNotifiesAndInputTest( int32 InMontageInstanceId );
private:
	// 발동할 스킬 애니메이션, 누르면 바로 작동
	UPROPERTY( Replicated, EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo ComboSkillAnimInfo;

	// 콤보 스킬에 관련한 정보. Combo Skill Anim Info의 Anim에서 정보를 파싱.
	// sort by notify number
	UPROPERTY( EditAnywhere, Category = "Skill|Anim", meta = (EditFixedOrder) )
	TArray<FR4ComboInputInfo> ComboInputInfo;

	//// Caching ////
	
	// Combo Skill Input 을 받을 수 있나?
	uint8 CachedCanComboInput:1;
	
	// Combo Input이 입력된 상태인지?
	uint8 CachedOnComboInput:1;
};
