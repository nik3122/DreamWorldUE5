// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Inventory.h"
#include "Inventory/CharacterInventory.h"
#include "Inventory/InventorySlot.h"
#include "Widget/Inventory/WidgetInventorySkillSlot.h"

UWidgetInventoryBar::UWidgetInventoryBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("InventoryBar");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameOnly;
	UISkillSlots = TArray<UWidgetInventorySkillSlot*>();
	SelectedSlotIndex = 0;
	OwnerCharacter = nullptr;
}

void UWidgetInventoryBar::SetActive(bool bActive)
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

void UWidgetInventoryBar::SetOwnerCharacter(ADWPlayerCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	if (OwnerCharacter)
	{
		InitInventory(OwnerCharacter->GetInventory());
	}
	else
	{
		SetActive(false);
	}
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
