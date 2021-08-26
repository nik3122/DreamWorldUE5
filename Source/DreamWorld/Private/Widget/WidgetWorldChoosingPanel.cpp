// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetWorldChoosingPanel.h"

UWidgetWorldChoosingPanel::UWidgetWorldChoosingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("WorldChoosingPanel");
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::UIOnly;
}
