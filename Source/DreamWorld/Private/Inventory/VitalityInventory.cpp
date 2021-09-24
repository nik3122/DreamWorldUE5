// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/VitalityInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Inventory/Slot/InventoryEquipSlot.h"
#include "DWGameInstance.h"
#include "VitalityObject.h"
#include "WidgetModuleBPLibrary.h"

UVitalityInventory::UVitalityInventory()
{
	
}

void UVitalityInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
	Super::Initialize(InOwner, InSplitInfos);
}

void UVitalityInventory::LoadData(FInventoryData InInventoryData, AActor* InOwner)
{
	Super::LoadData(InInventoryData, InOwner);
}

FInventoryData UVitalityInventory::ToData(bool bSaved)
{
	return Super::ToData();
}

void UVitalityInventory::DiscardAll()
{
	Super::DiscardAll();
}

void UVitalityInventory::ClearAll()
{
	Super::ClearAll();
}

TArray<UInventorySlot*> UVitalityInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int32 InStartIndex, int32 InEndIndex)
{
	return Super::GetValidatedList(InActionType, InItem, InStartIndex, InEndIndex);
}

AVitalityObject* UVitalityInventory::GetOwnerVitality() const
{
	return Cast<AVitalityObject>(OwnerActor);
}
