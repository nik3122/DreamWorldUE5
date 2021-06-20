// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Inventory/CharacterInventory.h"

UWidgetInventoryPanel::UWidgetInventoryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("InventoryPanel");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameAndUI;
	UIEquipSlots = TArray<UWidgetInventorySlot*>();
	OwnerCharacter = nullptr;
}

void UWidgetInventoryPanel::SetActive(bool bActive)
{
	if (OwnerCharacter)
	{
		Super::SetActive(bActive);
	}
	else
	{
		Super::SetActive(false);
	}
}

void UWidgetInventoryPanel::SetOwnerCharacter(ADWPlayerCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	if (OwnerCharacter)
	{
		InitInventory(InOwnerCharacter->GetInventory());
	}
	else
	{
		SetActive(false);
	}
}
