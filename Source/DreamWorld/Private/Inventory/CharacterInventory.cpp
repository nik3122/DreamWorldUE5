// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/CharacterInventory.h"
#include "Inventory/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Inventory/InventorySlot.h"
#include "Inventory/InventoryEquipSlot.h"
#include "Inventory/InventorySkillSlot.h"
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
