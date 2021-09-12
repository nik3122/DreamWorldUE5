// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetWorldCreatingPanel.h"

UWidgetWorldCreatingPanel::UWidgetWorldCreatingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::UIOnly;
}

void UWidgetWorldCreatingPanel::OnRefresh_Implementation()
{
	Super::OnRefresh_Implementation();
	
	if(IsVisible())
	{
		ResetData();
	}
}
