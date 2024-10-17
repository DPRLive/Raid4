// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4MainMenuWidget.generated.h"

struct FR4SessionEntryInfo;
class UEditableText;
class UCheckBox;
class UButton;
class UListView;
/**
 * Main Menu Widget.
 */
UCLASS()
class RAID4_API UR4MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UR4MainMenuWidget( const FObjectInitializer& ObjectInitializer );

protected:
	virtual void NativeConstruct() override;

private:
	// _OnClickNewGameButton
	UFUNCTION()
	void _OnClickNewGameButton();

	// _OnClickFindGameButton
	UFUNCTION()
	void _OnClickFindGameButton();

	// _OnClickJoinButton (SessionList's widget)
	void _OnDoubleClickSessionEntry( UObject* InObject ) const;
	
	// Session 찾기 결과를 수신.
	void _OnFindSessionCompleted( const TArray<FOnlineSessionSearchResult>& InResult );

	// Player Name Parsing
	FString _ParsePlayerName() const;
private:
	// Player Name
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UEditableText> PlayerNameText;

	// Is Lan Match
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UCheckBox> IsLanMatchCheckBox;
	
	// Session ListView
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UListView> SessionList;

	// New Game button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> NewGameButton;

	// Find Game button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> FindGameButton;
	
	// Session을 한번에 얼마나 찾을지 설정
	UPROPERTY( EditDefaultsOnly )
	int32 MaxSessionSearchNum;
};
