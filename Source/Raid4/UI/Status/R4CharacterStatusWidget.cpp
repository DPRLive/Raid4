// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterStatusWidget.h"
#include "R4NameplateWidget.h"
#include "R4NameplateWidgetInterface.h"
#include "R4HpBarWidget.h"
#include "R4HpBarWidgetInterface.h"

#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterStatusWidget)

UR4CharacterStatusWidget::UR4CharacterStatusWidget( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
}

void UR4CharacterStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( !OwningActor.IsValid() )
		return;
	
	// Nameplate Widget
	if( IsValid( NameplateWidget ) )
	{
		if ( IR4NameplateWidgetInterface* owner = Cast<IR4NameplateWidgetInterface>( OwningActor ) )
			owner->SetupNameplateWidget( NameplateWidget );
	}
	
	// HP Bar Setup
	if( IsValid( HpBarWidget ) )
	{
		if ( IR4HpBarWidgetInterface* owner = Cast<IR4HpBarWidgetInterface>( OwningActor ) )
			owner->SetupHpBarWidget( HpBarWidget );
	}
}