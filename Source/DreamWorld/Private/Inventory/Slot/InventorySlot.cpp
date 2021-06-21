// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Slot/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "World/WorldManager.h"
#include "World/Chunk.h"
#include "DWGameInstance.h"
#include "InventoryEquipSlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Vitality/Vitality.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Abilities/Item/DWItemAbility.h"

UInventorySlot::UInventorySlot()
{
	Item = FItem::Empty;
	UISlot = nullptr;
	Owner = nullptr;
	LimitType = EItemType::None;
	SplitType = ESplitSlotType::Default;
	AbilityHandle = FGameplayAbilitySpecHandle();
}

void UInventorySlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /*= EItemType::None*/, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Item = InItem;
	Owner = InOwner;
	LimitType = InLimitType;
	SplitType = InSplitType;
}

void UInventorySlot::SetUISlot(UWidgetInventorySlot* InUISlot)
{
	UISlot = InUISlot;
	UISlot->Refresh();
}

bool UInventorySlot::CheckSlot(FItem InItem)
{
	return LimitType == EItemType::None || LimitType == InItem.GetData().Type;
}

bool UInventorySlot::CanPutIn(FItem InItem)
{
	return (IsEmpty() && CheckSlot(InItem)) || (Item.Equal(InItem) && GetRemainVolume() > 0);
}

bool UInventorySlot::Contains(FItem InItem)
{
	return !IsEmpty() && Item.Equal(InItem);
}

void UInventorySlot::Refresh()
{
	if (Item.Count == 0) Item = FItem::Empty;
	if (UISlot) UISlot->Refresh();
	if (UDWHelper::GetWidgetInventoryBar()->GetSelectedSlot() == this)
	{
		UDWHelper::GetPlayerCharacter()->UpdateVoxelMesh();
	}
}

void UInventorySlot::SetItem(FItem& InItem, bool bReplace)
{
	if (InItem.IsValid())
	{
		if (!bReplace)
		{
			Item = FItem::Clone(InItem);
			int tmpNum = InItem.Count - GetMaxVolume();
			Item.Count = FMath::Clamp(Item.Count, 0, GetMaxVolume());
			InItem.Count = tmpNum;

			IVitality* vitality = Cast<IVitality>(Owner->GetOwnerActor());
			if (vitality && InItem.GetData().AbilityClass)
			{
				AbilityHandle = vitality->AcquireAbility(InItem.GetData().AbilityClass);
			}
		}
		else
		{
			FItem tmpItem = Item;
			Item = InItem;
			InItem = tmpItem;
		}
	}
	else
	{
		Item = InItem;
		AbilityHandle = FGameplayAbilitySpecHandle();
	}
	Refresh();
}

void UInventorySlot::AddItem(FItem& InItem)
{
	if (Contains(InItem))
	{
		if (GetRemainVolume() > 0)
		{
			int tmpNum = InItem.Count - GetRemainVolume();
			Item.Count = FMath::Clamp(Item.Count + InItem.Count, 0, GetMaxVolume());
			InItem.Count = tmpNum;
			Refresh();
		}
		else
		{
			SetItem(InItem, true);
		}
	}
	else
	{
		SetItem(InItem);
	}
}

void UInventorySlot::SubItem(FItem& InItem)
{
	if (Contains(InItem))
	{
		int tmpNum = InItem.Count - Item.Count;
		Item.Count = FMath::Clamp(Item.Count - InItem.Count, 0, GetMaxVolume());
		InItem.Count = tmpNum;
		Refresh();
	}
}

void UInventorySlot::SplitItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if(InCount == - 1) InCount = Item.Count;
	int tmpCount = Item.Count / InCount;
	auto tmpList = Owner->GetValidatedList(EInventoryActionType::Addition, Item);
	for (int i = 0; i < InCount; i++)
	{
		FItem tmpItem = FItem::Clone(Item, tmpCount);
		Item.Count -= tmpItem.Count;
		for (int j = 0; j < tmpList.Num(); j++)
		{
			if (tmpList[j]->IsEmpty() && tmpList[j] != this)
			{
				tmpList[j]->SetItem(tmpItem);
				tmpList.RemoveAt(j);
				break;
			}
		}
	}
	Refresh();
}

void UInventorySlot::MoveItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if (InCount == -1) InCount = Item.Count;
	FItem tmpItem = FItem::Clone(Item, InCount);

	SubItem(tmpItem);

	if(Owner->GetConnectInventory())
	{
		Owner->GetConnectInventory()->AdditionItems(tmpItem);
	}
	else if(Owner->GetSplitSlotInfos().Num() > 0)
	{
		
	}

	if (InCount != -1)
	{
		Item.Count -= InCount;
		Refresh();
	}
	else
	{
		ClearItem();
	}
}

void UInventorySlot::UseItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if (InCount == -1) InCount = Item.Count;
	FItem tmpItem = FItem::Clone(Item, InCount);
	switch (Item.GetData().Type)
	{
		case EItemType::Voxel:
		{
			SubItem(tmpItem);
			break;
		}
		case EItemType::Prop:
		{
			SubItem(tmpItem);
			IVitality* tmpVitality = Cast<IVitality>(Owner->GetOwnerActor());
			if (tmpVitality)
			{
				tmpVitality->ActiveAbility(this);
			}
			break;
		}
		case EItemType::Equip:
		{
			TArray<UInventoryEquipSlot*> EquipSlots = Owner->GetSplitSlots<UInventoryEquipSlot>(ESplitSlotType::Equip);
			if(EquipSlots.IsValidIndex((int32)UDWHelper::LoadEquipData(Item.ID).PartType))
			{
				UInventoryEquipSlot* EquipSlot = EquipSlots[(int32)UDWHelper::LoadEquipData(Item.ID).PartType];
				if(EquipSlot->CanPutIn(tmpItem))
				{
					SubItem(tmpItem);
					EquipSlot->AddItem(tmpItem);
				}
			}
			break;
		}
	}
}

void UInventorySlot::DiscardItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if (InCount == -1) InCount = Item.Count;
	FItem tmpItem = FItem::Clone(Item, InCount);
	auto chunk = UDWHelper::GetWorldManager()->FindChunk(Owner->GetOwnerActor()->GetActorLocation());
	if (chunk != nullptr)
	{
		chunk->SpawnPickUp(tmpItem, Owner->GetOwnerActor()->GetActorLocation() + FMath::RandPointInBox(FBox(FVector(-20, -20, -10), FVector(20, 20, 10))));
	}
	SubItem(tmpItem);
}

void UInventorySlot::ClearItem()
{
	if (IsEmpty()) return;

	SetItem(FItem::Empty);
}

bool UInventorySlot::IsEmpty() const
{
	return Item == FItem::Empty;
}

int UInventorySlot::GetRemainVolume() const
{
	return GetMaxVolume() - Item.Count;
}

int UInventorySlot::GetMaxVolume() const
{
	return Item.GetData().MaxCount;
}
