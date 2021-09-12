// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Slot/InventoryShortcutSlot.h"

#include "DWPlayerCharacter.h"
#include "WidgetInventoryBar.h"
#include "Inventory/Inventory.h"
#include "Inventory/Slot/InventorySlot.h"

UInventoryShortcutSlot::UInventoryShortcutSlot()
{
	LimitType = EItemType::Skill;
}

void UInventoryShortcutSlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Super::InitSlot(InOwner, InItem, InLimitType, InSplitType);
}

void UInventoryShortcutSlot::Refresh()
{
	Super::Refresh();
	if(ADWPlayerCharacter* PlayerCharacter = Cast<ADWPlayerCharacter>(GetOwner()->GetOwnerActor()))
	{
		if (UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedSlot() == this)
		{
			PlayerCharacter->UpdateVoxelMesh();
		}
	}
}
