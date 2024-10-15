// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4CharacterPickerWidget.generated.h"

class UR4ImageTextHorizontalBox;
class UVerticalBox;
class UR4ImageIdButton;
class UButton;
class UHorizontalBox;

/**
 * PlayerState 감시 정보.
 */
USTRUCT( BlueprintType )
struct FR4PlayerStateMonitorInfo
{
	GENERATED_BODY()

	FR4PlayerStateMonitorInfo()
	: PlayerStatePtr( nullptr )
	, CharacterIdHandle( FDelegateHandle() )
	, PlayerNameHandle( FDelegateHandle() )
	{}

	// 감시중인 Player State Ptr
	UPROPERTY( Transient, VisibleInstanceOnly )
	TWeakObjectPtr<APlayerState> PlayerStatePtr;

	// On Set Character id Delegate handle
	FDelegateHandle CharacterIdHandle;
	
	// On Set PlayerName Delegate Handle
	FDelegateHandle PlayerNameHandle;
};

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
	// Character Pick
	UFUNCTION()
	void _OnClickCharacterPortraitButton( int32 InCharacterId );
	
	// Character Fix.
	UFUNCTION()
	void _OnClickCharacterFixButton();

	// Game Start. ( Server )
	UFUNCTION()
	void _OnClickGameStartButton();
	
	// create & add portrait button
	void _CreateCharacterPickButton( int32 InCharacterId );

	// On Change Player State.
	void _OnChangePlayerStateArray();

	//  Player State 감시를 추가 + PlayerPickBox 동기화.
	void _AddPlayerStateMonitor( APlayerState* InPlayerState );
	
	// PlayerState의 Character가 Fix 되었을 때 호출
	void _OnFixCharacterId( int32 InMonitorIndex, int32 InCharacterId );

	// PlayerState의 PlayerName이 Set 되었을 때 호출
	void _OnSetPlayerName( int32 InMonitorIndex, const FString& InName );

	// Character DT Key에 맞는 Portrait return
	const TSoftObjectPtr<UTexture>* _GetCharacterPortrait( int32 InCharacterId ) const;
private:
	// Character Pick (Button)을 담는 Character List Box.
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UHorizontalBox> PortraitButtonBox;

	// Player들을 표시하는 Player List Box.
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UVerticalBox> PlayerPickBox;
	
	// Character Pick 확정 Button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> CharacterFixButton;

	// Game Start Button ( Server Only )
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> GameStartButton;

	// Character Picker로 사용할 button
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4ImageIdButton> CharacterPortraitButtonClass;

	// Player의 Character Pick을 보여줄 Element
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4ImageTextHorizontalBox> PlayerPickBoxElemClass;
	
	// 현재 선택된 Character ID.
	int32 CachedPickedCharacterId;

	// 감시 중인 Player State Info Array. Player Pick Box의 Element, Game State의 PlayerArray와 동기화.
	TArray<FR4PlayerStateMonitorInfo> CachedPlayerStateMonitors;
};
