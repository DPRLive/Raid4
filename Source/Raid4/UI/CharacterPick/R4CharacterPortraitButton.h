// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/Button.h>

#include "R4CharacterPortraitButton.generated.h"

/**
 * Character DT Key를 Push하면 Portrait를 Load하여 표시하는 button
 * OnPortraitButtonClicked 를 사용 할 것 !
 */
UCLASS()
class RAID4_API UR4CharacterPortraitButton : public UButton
{
	GENERATED_BODY()

public:
	UR4CharacterPortraitButton();

public:
	// 해당 클래스에 알맞는 DT의 Pk를 통해 데이터를 push
	virtual void SetCharacterDTKey( FPriKey InPk );

	// Click 시 PK를 담아 호출.
	DECLARE_MULTICAST_DELEGATE_OneParam( FOnPortraitButtonClickedDelegate, int32 )
	FOnPortraitButtonClickedDelegate OnPortraitButtonClickedDelegate;
	
private:
	// Texture Async Load 성공 시 호출
	UFUNCTION()
	void _TextureLoadComplete();

	// Button Click 시 DT key를 담아 broadcast.
	UFUNCTION()
	void _OnPortraitButtonClicked();
	
private:
	// 선택된 Character ID
	UPROPERTY( Transient, VisibleInstanceOnly )
	int32 CachedCharacterId;
	
	// Texture Async load handle
	TSharedPtr<FStreamableHandle> CachedTextureHandle;
};
