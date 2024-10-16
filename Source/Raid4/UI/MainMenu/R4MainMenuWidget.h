// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4MainMenuWidget.generated.h"

class UEditableText;
class UCheckBox;
class UWidgetSwitcher;
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

	// _OnClickCreateSessionButton
	UFUNCTION()
	void _OnClickCreateSessionButton();

	// _OnClickBackMainButton
	UFUNCTION()
	void _OnClickBackMainButton();

	// Session 찾기 결과를 수신.
	void _OnFindSessionCompleted( const TArray<FOnlineSessionSearchResult>& InResult );
private:
	// Widget Switcher
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	// Main Widget
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UWidget> MainWidget;

	// Session ListView
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UListView> SessionList;

	// New Game button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> NewGameButton;

	// Find Game button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> FindGameButton;

	// NewSession Widget
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UWidget> NewSessionWidget;
	
	// Session Name
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UEditableText> SessionNameText;

	// Session Name
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UCheckBox> IsLanMatchCheckBox;
	
	// Create Session Button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> CreateSessionButton;

	// Back Main Button
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> BackMainButton;

	// Session을 한번에 얼마나 찾을지 설정
	UPROPERTY( EditDefaultsOnly )
	int32 MaxSessionSearchNum;
};
