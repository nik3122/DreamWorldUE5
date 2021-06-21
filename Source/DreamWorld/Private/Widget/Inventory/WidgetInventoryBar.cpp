// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Inventory.h"
#include "Inventory/Character/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Widget/Inventory/Slot/WidgetInventorySkillSlot.h"

UWidgetInventoryBar::UWidgetInventoryBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("InventoryBar");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameOnly;
	SelectedSlotIndex = 0;
}

void UWidgetInventoryBar::PrevInventorySlot()
{
	if(SelectedSlotIndex > 0)
		SelectInventorySlot(SelectedSlotIndex - 1);
	else
		SelectInventorySlot(9);
}

void UWidgetInventoryBar::NextInventorySlot()
{
	if (SelectedSlotIndex < 9)
		SelectInventorySlot(SelectedSlotIndex + 1);
	else
		SelectInventorySlot(0);
}

void UWidgetInventoryBar::SelectInventorySlot(int32 InSlotIndex)
{
	SelectedSlotIndex = InSlotIndex;
	UpdateSelectBox();
}

UInventorySlot* UWidgetInventoryBar::GetSelectedSlot()
{
	auto UISlots = GetSplitUISlots(ESplitSlotType::Shortcut);
	if(UISlots.Num() > SelectedSlotIndex)
	{
		return UISlots[SelectedSlotIndex]->GetSlot();
	}
	return nullptr;
}

FItem UWidgetInventoryBar::GetSelectedItem()
{
	if (GetSelectedSlot())
	{
		return GetSelectedSlot()->GetItem();
	}
	return FItem::Empty;
}
