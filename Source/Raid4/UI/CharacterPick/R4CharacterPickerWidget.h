// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4CharacterPickerWidget.generated.h"

class UVerticalBox;
class UR4CharacterPortraitButton;
class UButton;
class UHorizontalBox;

/**
 * Character Picker.
 */
UCLASS()
class RAID4_API UR4CharacterPickerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	// Character Fix.
	UFUNCTION()
	void _OnClickCharacterFixButton();

	// Game Start. ( Server )
	UFUNCTION()
	void _OnClickGameStartButton();

	// Character가 Fix 되었을 때 호출
	UFUNCTION()
	void _OnFixCharacterId( int32 InCharacterId );

	// Character Pick
	UFUNCTION()
	void _OnClickCharacterPick( int32 InCharacterId );
	
	// create & add portrait button
	void _CreateCharacterPickButton( int32 InCharacterId );

	// On Change Player State.
	void _OnChangePlayerStateArray();

	// On New Player State.
	void _OnNewPlayerState( APlayerState* InPlayerState );
private:
	// Character Pick (Button)을 담는 Character List Box.
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UHorizontalBox> CharacterBox;

	// Player들을 표시하는 Player List Box.
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UVerticalBox> PlayerBox;
	
	// Character Pick 확정 Button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> CharacterFixButton;

	// Game Start Button ( Server Only )
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> GameStartButton;

	// Character Picker로 사용할 button
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4CharacterPortraitButton> CharacterPortraitButtonClass;
	
	// 현재 선택된 Character ID.
	int32 CachedPickedCharacterId;
};
