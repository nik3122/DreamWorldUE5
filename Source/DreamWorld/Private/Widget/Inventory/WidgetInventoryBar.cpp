// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryBar.h"

#include "Inventory.h"
#include "WidgetModuleBPLibrary.h"
#include "WidgetPrimaryPanel.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Slot/InventorySlot.h"

UWidgetInventoryBar::UWidgetInventoryBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("InventoryBar");
	WidgetType = EWidgetType::Permanent;
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
	GetInventory()->SetSelectedSlot(GetSelectedSlot());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->ShowMessage(GetSelectedItem().GetData().Name.ToString());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->RefreshOptions();
}

UInventorySlot* UWidgetInventoryBar::GetSelectedSlot() const
{
	auto UISlots = GetSplitUISlots(ESplitSlotType::Shortcut);
	if(UISlots.Num() > SelectedSlotIndex)
	{
		return UISlots[SelectedSlotIndex]->GetOwnerSlot();
	}
	return nullptr;
}

FItem UWidgetInventoryBar::GetSelectedItem() const
{
	if (GetSelectedSlot())
	{
		return GetSelectedSlot()->GetItem();
	}
	return FItem::Empty;
}
