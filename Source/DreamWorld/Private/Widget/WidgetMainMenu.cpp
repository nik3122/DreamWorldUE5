// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetMainMenu.h"

UWidgetMainMenu::UWidgetMainMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("MainMenu");
	WidgetType = EWidgetType::Temporary;
	InputMode = EInputMode::UIOnly;
}
