// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4InGameHUDWidget.generated.h"

class UR4ImageIdButton;
class UHorizontalBox;
class UTextBlock;
class UR4HpBarWidget;

/**
 * InGame에서 사용되는 HUD.
 */
UCLASS()
class RAID4_API UR4InGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UR4InGameHUDWidget( const FObjectInitializer& ObjectInitializer );

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
public:
	// Cooldown Update시 호출
	void OnUpdateSkillCooldown();
	
private:
	// Skill Box Update 호출
	void _OnUpdateSkillBox();
	
	// Create Skill Box
	void _CreateSkillBox( int32 InCharacterId );

	// HpBar Update시 호출
	void _OnUpdateHpBar() const;
private:
	// Hp Bar
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UR4HpBarWidget> HpBarWidget;

	// TotalHp Text
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> TotalHpText;
	
	// NowHp Text
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> CurrentHpText;

	// Skill Box
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UHorizontalBox> SkillBox;

	// Skill cooldown update interval
	UPROPERTY( EditDefaultsOnly )
	float SkillCooldownInterval;
	
	// Skill slot
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UR4ImageIdButton> SkillSlotClass;

	// Skill Slot Padding
	UPROPERTY( EditDefaultsOnly )
	FMargin SkillSlotPadding;

	// Skill Slot Size ( Width, Height )
	UPROPERTY( EditDefaultsOnly )
	FVector2D SkillSlotSize;
	
	// Skill Cooldown text class
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UTextBlock> SkillCooldownTextClass;
	
	// Skill CoolDowns
	UPROPERTY( Transient, VisibleInstanceOnly )
	TArray<TWeakObjectPtr<UTextBlock>> CachedSkillCooldowns;
	
	// Cooldown Update를 위한 Timer handle
	FTimerHandle SkillCooldownTimerHandle;
};