// Fill out your copyright notice in the Description page of Project Settings.


#include "R4InGameHUDWidget.h"

#include "../R4ImageIdButton.h"
#include "../Status/R4HpBarWidgetInterface.h"
#include "../Status/R4HpBarWidget.h"
#include "../Status/R4HUDWidgetInterface.h"
#include "../../Data/Character/R4CharacterRow.h"
#include "../../Data/Character/R4CharacterSrcRow.h"
#include "../../PlayerState/R4PlayerStateInterface.h"

#include <GameFramework/Pawn.h>
#include <Components/TextBlock.h>
#include <Components/HorizontalBox.h>
#include <Components/HorizontalBoxSlot.h>
#include <Components/SizeBox.h>
#include <GameFramework/PlayerState.h>
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4InGameHUDWidget)

UR4InGameHUDWidget::UR4InGameHUDWidget( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	SkillCooldownInterval = 0.f;

	SkillSlotPadding = FMargin();
}

void UR4InGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// HP Bar Setup
	if( IsValid( HpBarWidget ) )
	{
		if ( IR4HpBarWidgetInterface* owner = Cast<IR4HpBarWidgetInterface>( GetOwningPlayerPawn() ) )
		{
			HpBarWidget->OnHpBarUpdate.AddWeakLambda( this, [this]()
			{
				_OnUpdateHpBar();
			} );
		
			owner->SetupHpBarWidget( HpBarWidget );
		}
	}

	// Create Skill Box
	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( GetOwningPlayerState() ) )
		_CreateSkillBox( playerState->GetCharacterId() );

	// Skill Set up
	if ( IR4HUDWidgetInterface* owner = Cast<IR4HUDWidgetInterface>( GetOwningPlayerPawn() ) )
		owner->SetupSkillWidget( this );
}

void UR4InGameHUDWidget::NativeDestruct()
{
	if( GetWorld() )
		GetWorld()->GetTimerManager().ClearTimer( SkillCooldownTimerHandle );
	
	Super::NativeDestruct();
}

/**
 *  Cooldown Update시 호출
 */
void UR4InGameHUDWidget::OnUpdateSkillCooldown()
{
	// Timer를 통한 지속적인 Cooldown Update.
	if( GetWorld() )
	{
		GetWorld()->GetTimerManager().SetTimer( SkillCooldownTimerHandle,
			[thisPtr = TWeakObjectPtr<UR4InGameHUDWidget>(this)]()
		{
			if( thisPtr.IsValid() )
				thisPtr->_OnUpdateSkillBox();	
		}, SkillCooldownInterval, true );
	}
}

/**
 *  HpBar Update시 호출
 */
void UR4InGameHUDWidget::_OnUpdateSkillBox()
{
	bool bNeedUpdate = false;
	
	if ( IR4HUDWidgetInterface* owner = Cast<IR4HUDWidgetInterface>( GetOwningPlayerPawn() ) )
	{
		for( int32 idx = 0; idx < CachedSkillCooldowns.Num(); idx++ )
		{
			float cooldown = owner->GetSkillCooldownRemaining( idx );
			USizeBox* skillSlotWrapper = Cast<USizeBox>( SkillBox->GetChildAt( idx ) );
			UWidget* skillSlot = nullptr;
			if( IsValid( skillSlotWrapper ) )
				skillSlot = skillSlotWrapper->GetChildAt( 0 );
			
			if( !CachedSkillCooldowns.IsValidIndex( idx ) || !CachedSkillCooldowns[idx].IsValid() )
				continue;

			if( !IsValid( skillSlot ) )
				continue;

			if ( cooldown < KINDA_SMALL_NUMBER )
			{
				skillSlot->SetIsEnabled( true );
				CachedSkillCooldowns[idx]->SetVisibility( ESlateVisibility::Hidden );
				continue;
			}

			bNeedUpdate |= true;
			
			skillSlot->SetIsEnabled( false );
			CachedSkillCooldowns[idx]->SetVisibility( ESlateVisibility::HitTestInvisible );
			CachedSkillCooldowns[idx]->SetText( FText::FromString( FString::Printf(TEXT( "%.1f" ), cooldown) ) );
		}
	}

	if ( !bNeedUpdate )
	{
		if( GetWorld() )
			GetWorld()->GetTimerManager().ClearTimer( SkillCooldownTimerHandle );
	}
}

/**
 *  Create Skill Box
 */
void UR4InGameHUDWidget::_CreateSkillBox( int32 InCharacterId )
{
	const FR4CharacterRowPtr characterData( GetWorld(), InCharacterId );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InCharacterId );
		return;
	}

	// Get Resource Pk
	const FR4CharacterSrcRowPtr characterSrcRow( GetWorld(), characterData->ResourceRowPK );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterSrcData is Invalid. PK : [%d]"), InCharacterId );
		return;
	}

	for ( int32 idx = 0; idx < characterData->Skills.Num(); idx++ )
	{
		USizeBox* slotWrapper = NewObject<USizeBox>( this ); 
		UR4ImageIdButton* newSkillSlot = NewObject<UR4ImageIdButton>( this, SkillSlotClass );
		UTextBlock* cooldownText = NewObject<UTextBlock>( this, SkillCooldownTextClass );
		
		if ( IsValid( slotWrapper ) && IsValid( newSkillSlot ) && IsValid( cooldownText ) )
		{
			if ( characterSrcRow->SkillIcons.IsValidIndex( idx ) )
				newSkillSlot->SetImage( characterSrcRow->SkillIcons[idx] );

			// SkillBox ( SizeBox ( Button ( TextBlock ) )
			cooldownText->SetJustification( ETextJustify::Center );

			newSkillSlot->AddChild( cooldownText );
			
			slotWrapper->SetWidthOverride( SkillSlotSize.X );
			slotWrapper->SetHeightOverride( SkillSlotSize.Y );
			slotWrapper->AddChild( newSkillSlot );

			SkillBox->AddChild( slotWrapper );

			CachedSkillCooldowns.Add( cooldownText );
		}
	}

	// align & apply padding
	for( UPanelSlot* slot : SkillBox->GetSlots() )
	{
		if ( UHorizontalBoxSlot* boxSlot = Cast<UHorizontalBoxSlot>( slot ) )
		{
			boxSlot->SetHorizontalAlignment( HAlign_Center );
			boxSlot->SetVerticalAlignment( VAlign_Center );
			boxSlot->SetPadding( SkillSlotPadding );
		}
	}
}

/**
 *  HpBar Update시 호출
 */
void UR4InGameHUDWidget::_OnUpdateHpBar() const
{
	if ( IsValid( HpBarWidget ) && IsValid( TotalHpText ) && IsValid( CurrentHpText ) )
	{
		TotalHpText->SetText( FText::FromString( FString::Printf( TEXT( "%.1f" ), HpBarWidget->GetTotalHp() ) ) );
		CurrentHpText->SetText( FText::FromString( FString::Printf( TEXT( "%.1f" ), HpBarWidget->GetCurrentHp() ) ) );
	}
}