// Fill out your copyright notice in the Description page of Project Settings.

#include "R4WidgetComponent.h"
#include "R4UserWidget.h"

#include <Framework/Application/SlateApplication.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4WidgetComponent)

void UR4WidgetComponent::InitWidget()
{
	Super::InitWidget();

	// 설정 할 위젯이 R4UserWidget이면 Owner를 설정.
	if ( UR4UserWidget* widget = Cast<UR4UserWidget>( GetWidget() ) )
		widget->SetOwningActor( GetOwner() );
}

void UR4WidgetComponent::SetWidget( UUserWidget* InWidget )
{
	// 설정 할 위젯이 R4UserWidget이면 Owner를 설정. ( In Runtime )
	if ( UR4UserWidget* widget = Cast<UR4UserWidget>( InWidget ) )
		widget->SetOwningActor( GetOwner() );
	
	Super::SetWidget( InWidget );
}
