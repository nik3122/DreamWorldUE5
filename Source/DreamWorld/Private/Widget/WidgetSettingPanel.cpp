// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetSettingPanel.h"

UWidgetSettingPanel::UWidgetSettingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("SettingPanel");
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::UIOnly;
}
