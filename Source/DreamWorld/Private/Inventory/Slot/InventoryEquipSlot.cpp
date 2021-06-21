// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Slot/InventoryEquipSlot.h"
#include "Inventory/Inventory.h"
#include "Character/DWCharacter.h"

UInventoryEquipSlot::UInventoryEquipSlot()
{
	LimitType = EItemType::Equip;
	PartType = EEquipPartType::Head;
}

void UInventoryEquipSlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Super::InitSlot(InOwner, InItem, InLimitType, InSplitType);
}

bool UInventoryEquipSlot::CheckSlot(FItem InItem)
{
	return Super::CheckSlot(InItem) && UDWHelper::LoadEquipData(InItem.ID).PartType == PartType;
}

void UInventoryEquipSlot::Refresh()
{
	Super::Refresh();
	auto character = Cast<ADWCharacter>(Owner->GetOwnerActor());
	if (character != nullptr)
	{
		character->RefreshEquip(PartType, this);
	}
}

void UInventoryEquipSlot::UseItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	MoveItem(InCount);
}
