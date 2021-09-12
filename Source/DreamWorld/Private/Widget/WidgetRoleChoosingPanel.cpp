// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetRoleChoosingPanel.h"

UWidgetRoleChoosingPanel::UWidgetRoleChoosingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::UIOnly;
}
