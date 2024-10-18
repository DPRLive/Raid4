// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4UserWidget.h"
#include "R4NameplateWidget.generated.h"

class UTextBlock;

/**
 * Nameplate Widget
 */
UCLASS()
class RAID4_API UR4NameplateWidget : public UR4UserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	// 이름을 설정
	void SetName( const FString& InName ) const;
	
private:
	// Nameplate Text
	UPROPERTY( BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true) )
	TObjectPtr<UTextBlock> NameplateText;
};
