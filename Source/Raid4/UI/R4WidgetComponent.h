// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/WidgetComponent.h>
#include "R4WidgetComponent.generated.h"

/**
 * R4Userwidget에 Owner를 설정해주는 친절한 widget component 클래스
 */
UCLASS(Blueprintable, ClassGroup="UserInterface", hidecategories=(Object,Activation,"Components|Activation",Sockets,Base,Lighting,LOD,Mesh), editinlinenew, meta=(BlueprintSpawnableComponent))
class RAID4_API UR4WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitWidget() override;

	virtual void SetWidget( UUserWidget* InWidget ) override;
};
