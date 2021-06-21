// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Character/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Inventory/Slot/InventoryEquipSlot.h"
#include "Inventory/Slot/InventorySkillSlot.h"
#include "Character/DWCharacter.h"
#include "Vitality/Vitality.h"
#include "Abilities/Item/DWItemAbility.h"

UCharacterInventory::UCharacterInventory()
{
	
}

void UCharacterInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
	Super::Initialize(InOwner, InSplitInfos);
}

void UCharacterInventory::LoadData(FInventoryData InInventoryData, AActor* InOwner)
{
	Super::LoadData(InInventoryData, InOwner);
}

FInventoryData UCharacterInventory::ToData()
{
	return Super::ToData();
}

void UCharacterInventory::DiscardAll()
{
	Super::DiscardAll();
}

void UCharacterInventory::ClearAll()
{
	Super::ClearAll();
}

ADWCharacter* UCharacterInventory::GetOwnerCharacter() const
{
	return Cast<ADWCharacter>(OwnerActor);
}
