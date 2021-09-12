// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Inventory/Character/CharacterInventory.h"

UWidgetInventoryPanel::UWidgetInventoryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetType = EWidgetType::Permanent;
	InputMode = EInputMode::GameAndUI;
}
