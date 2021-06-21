// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetRoleCreatingPanel.h"

UWidgetRoleCreatingPanel::UWidgetRoleCreatingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("RoleCreatingPanel");
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::UIOnly;
}

void UWidgetRoleCreatingPanel::RefreshPanel()
{
	if(IsVisible())
	{
		ResetData();
	}
}
