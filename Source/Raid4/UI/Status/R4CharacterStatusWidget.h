// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4UserWidget.h"
#include "R4CharacterStatusWidget.generated.h"

class UR4NameplateWidget;
class UR4HpBarWidget;

/**
 * Character의 상태를 표시해주는 Character Status Widget
 * ( Name Plate + Hp Bar )
 */
UCLASS()
class RAID4_API UR4CharacterStatusWidget : public UR4UserWidget
{
	GENERATED_BODY()

public:
	UR4CharacterStatusWidget( const FObjectInitializer& ObjectInitializer );

protected:
	virtual void NativeConstruct() override;
	
private:
	// Nameplate
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UR4NameplateWidget> NameplateWidget;
	
	// Hp Bar
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UR4HpBarWidget> HpBarWidget;
};
