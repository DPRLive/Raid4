// Fill out your copyright notice in the Description page of Project Settings.


#include "R4InGameHUDWidget.h"

#include "../Status/R4HpBarWidgetInterface.h"
#include "../Status/R4HpBarWidget.h"

#include <GameFramework/Pawn.h>
#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4InGameHUDWidget)

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
}

/**
 *  HpBar Update시 호출
 */
void UR4InGameHUDWidget::_OnUpdateHpBar() const
{
	if ( IsValid( HpBarWidget ) && IsValid( TotalHpText ) && IsValid( CurrentHpText ) )
	{
		TotalHpText->SetText( FText::FromString( FString::SanitizeFloat( HpBarWidget->GetTotalHp() ) ) );
		CurrentHpText->SetText( FText::FromString( FString::SanitizeFloat( HpBarWidget->GetCurrentHp() ) ) );
	}
}