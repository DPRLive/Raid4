// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4InGameHUDWidget.generated.h"

class UTextBlock;
class UR4HpBarWidget;

/**
 * InGame에서 사용되는 HUD.
 */
UCLASS()
class RAID4_API UR4InGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
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
};