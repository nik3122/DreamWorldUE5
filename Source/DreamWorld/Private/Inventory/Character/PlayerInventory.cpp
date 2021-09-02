// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Character/PlayerInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Inventory/Slot/InventoryEquipSlot.h"
#include "DWGameInstance.h"

UPlayerInventory::UPlayerInventory()
{
	
}

void UPlayerInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
	Super::Initialize(InOwner, InSplitInfos);
}

TArray<UInventorySlot*> UPlayerInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int InStartIndex /*= 0*/)
{
	if (InStartIndex == -1) InStartIndex = GetSplitSlotInfo(ESplitSlotType::Shortcut).StartIndex + UDWHelper::GetWidgetPanelByClass<UWidgetInventoryBar>(this)->GetSelectedSlotIndex();
	return Super::GetValidatedList(InActionType, InItem, InStartIndex);
}
