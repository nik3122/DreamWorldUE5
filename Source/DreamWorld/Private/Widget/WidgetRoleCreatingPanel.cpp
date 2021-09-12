// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetRoleCreatingPanel.h"

UWidgetRoleCreatingPanel::UWidgetRoleCreatingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::UIOnly;
}

void UWidgetRoleCreatingPanel::OnRefresh_Implementation()
{
	Super::OnRefresh_Implementation();
	if(IsVisible())
	{
		ResetData();
	}
}