// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryBox.h"

UWidgetInventoryBox::UWidgetInventoryBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("InventoryBox");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameAndUI;
}
