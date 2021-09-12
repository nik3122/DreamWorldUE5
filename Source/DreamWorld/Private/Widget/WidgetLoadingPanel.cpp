// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetLoadingPanel.h"

UWidgetLoadingPanel::UWidgetLoadingPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::None;
}
