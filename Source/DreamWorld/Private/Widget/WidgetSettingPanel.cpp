// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetSettingPanel.h"

UWidgetSettingPanel::UWidgetSettingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::UIOnly;
}
