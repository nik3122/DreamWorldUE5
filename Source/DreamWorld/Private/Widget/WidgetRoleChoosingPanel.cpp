// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetRoleChoosingPanel.h"

UWidgetRoleChoosingPanel::UWidgetRoleChoosingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("RoleChoosingPanel");
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::UIOnly;
}

void UWidgetRoleChoosingPanel::RefreshPanel()
{
	if(IsVisible())
	{
		RefreshList();
	}
}
