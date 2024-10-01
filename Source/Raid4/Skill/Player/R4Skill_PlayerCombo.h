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

	FR4ComboInputInfo()
	: NowSectionIndex( INDEX_NONE )
	, NowSectionName( NAME_None )
	, NextSectionName( NAME_None )
	, InputTestDelay( 0.f )
	{}

	FR4ComboInputInfo( int32 InNowSectionIndex, const FName& InNowSectionName )
	: NowSectionIndex( InNowSectionIndex )
	, NowSectionName( InNowSectionName )
	, NextSectionName( NAME_None )
	, InputTestDelay( 0.f )
	{}
	
	// Notify가 현재 포함되어 있는 Section의 Index
    UPROPERTY( VisibleInstanceOnly )
    int32 NowSectionIndex;
	
	// 현재 Section의 Name
	UPROPERTY( VisibleAnywhere )
	FName NowSectionName;
	
	// 해당 Section의 Input 조건 만족 시 넘어갈 다음 Section을 설정
	UPROPERTY( EditAnywhere )
	FName NextSectionName;

	// 해당 섹션의 Combo Input Test Delay Time
	UPROPERTY( EditAnywhere )
	float InputTestDelay;
};

/**
 * Player Combo Skill
 * ( Section 별로 Combo Input Test 시,
 * Test 시점 전 입력이 진행되었으면 NextSection으로 Transition.
 * 아닌 경우 재생하던 Section을 끝까지 재생.)
 */
UCLASS( Abstract, Blueprintable, NotBlueprintType, ClassGroup=(Skill) )
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

	// 스킬 사용이 가능한지 판단
	virtual bool CanActivateSkill() const override;
	
protected:
	// Anim을 Play시작 시 호출.
	virtual void OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, float InStartServerTime ) override;

	// Anim Section Change 시 호출
	virtual void OnChangeSkillAnimSection( const FR4SkillAnimInfo& InSkillAnimInfo, FName InSectionName, float InStartChangeTime ) override;
	
	// Anim 종료 시 호출.
	virtual void OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted ) override;

	// Combo Input Test Execute 추가
	void AddInputTestExecute( int32 InSkillAnimKey, const FR4ComboInputInfo& InComboInputInfo, float InDelayRate = 1.f );
	
private:
	// Combo Input Test
	void _ComboInputTest( const FR4ComboInputInfo& InComboInputInfo );

	// Combo Input을 서버에 요청
	UFUNCTION( Server, Reliable )
	void _ServerRPC_RequestComboInput();

private:
	// 발동할 스킬 애니메이션, 누르면 바로 작동
	UPROPERTY( Replicated, EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo ComboSkillAnimInfo;

	// 콤보 스킬에 관련한 정보. Combo Skill Anim Info의 Anim에서 정보를 파싱.
	// sort by Section Index
	UPROPERTY( EditAnywhere, Category = "Skill|Anim", meta = (EditFixedOrder) )
	TArray<FR4ComboInputInfo> ComboInputInfo;

	//// Caching ////
	
	// Combo Skill Input 을 받을 수 있나?
	uint8 CachedCanComboInput:1;
	
	// Combo Input이 입력된 상태인지?
	uint8 CachedOnComboInput:1;
};
