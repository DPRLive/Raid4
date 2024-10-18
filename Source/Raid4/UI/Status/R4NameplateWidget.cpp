// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NameplateWidget.h"
#include "R4NameplateWidgetInterface.h"

#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NameplateWidget)

void UR4NameplateWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( !OwningActor.IsValid() )
		return;

	if ( IR4NameplateWidgetInterface* owner = Cast<IR4NameplateWidgetInterface>( OwningActor ) )
		owner->SetupNameplateWidget( this );
}

void UR4NameplateWidget::SetName( const FString& InName ) const
{
	if( IsValid( NameplateText ) )
		NameplateText->SetText( FText::FromString( InName ) );
}
