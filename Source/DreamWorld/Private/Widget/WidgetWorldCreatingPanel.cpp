// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetWorldCreatingPanel.h"

UWidgetWorldCreatingPanel::UWidgetWorldCreatingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("WorldCreatingPanel");
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::UIOnly;
}

void UWidgetWorldCreatingPanel::OnShowPanel()
{
	ResetData();
}
