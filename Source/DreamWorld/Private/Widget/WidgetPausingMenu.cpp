// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetPausingMenu.h"

UWidgetPausingMenu::UWidgetPausingMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("PausingMenu");
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::GameAndUI;
}
