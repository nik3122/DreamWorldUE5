// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Inventory.h"

#include "InventoryAuxiliarySlot.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Vitality/Vitality.h"
#include "InventoryEquipSlot.h"
#include "InventoryGenerateSlot.h"
#include "InventoryShortcutSlot.h"
#include "InventorySkillSlot.h"
#include "Abilities/Item/DWItemAbility.h"

UInventory::UInventory()
{
	OwnerActor = nullptr;
	ConnectInventory = nullptr;
	Slots = TArray<UInventorySlot*>();
}

void UInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
	for (auto Iter : Slots)
	{
		Iter->ConditionalBeginDestroy();
	}
	Slots.Empty();
	
	OwnerActor = InOwner;
	SplitInfos = InSplitInfos;
	for (auto Iter : SplitInfos)
	{
		for (int32 i = Iter.Value.StartIndex; i < Iter.Value.StartIndex + Iter.Value.TotalCount; i++)
		{
			UInventorySlot* Slot = nullptr;
			switch(Iter.Key)
			{
				case ESplitSlotType::Default:
				{
					Slot = NewObject<UInventorySlot>(OwnerActor);
					Slot->InitSlot(this, FItem::Empty, EItemType::None, Iter.Key);
					break;
				}
				case ESplitSlotType::Shortcut:
				{
					Slot = NewObject<UInventoryShortcutSlot>(OwnerActor);
					Slot->InitSlot(this, FItem::Empty, EItemType::None, Iter.Key);
					break;
				}
				case ESplitSlotType::Auxiliary:
				{
					Slot = NewObject<UInventoryAuxiliarySlot>(OwnerActor);
					Slot->InitSlot(this, FItem::Empty, EItemType::None, Iter.Key);
					break;
				}
				case ESplitSlotType::Generate:
				{
					Slot = NewObject<UInventoryGenerateSlot>(OwnerActor);
					Slot->InitSlot(this, FItem::Empty, EItemType::None, Iter.Key);
					break;
				}
				case ESplitSlotType::Equip:
				{
					Slot = NewObject<UInventoryEquipSlot>(OwnerActor);
					Cast<UInventoryEquipSlot>(Slot)->SetPartType((EEquipPartType)(i - Iter.Value.StartIndex));
					Slot->InitSlot(this, FItem::Empty, EItemType::Equip, Iter.Key);
					break;
				}
				case ESplitSlotType::Skill:
				{
					Slot = NewObject<UInventorySkillSlot>(OwnerActor);
					Slot->InitSlot(this, FItem::Empty, EItemType::Skill, Iter.Key);
					break;
				}
			}
			Slots.Add(Slot);
		}
	}
}

void UInventory::Refresh(float DeltaSeconds)
{
	for(int32 i = 0; i < Slots.Num(); i++)
	{
		if(Slots.IsValidIndex(i) && Slots[i])
		{
			Slots[i]->RefreshCooldown(DeltaSeconds);
		}
	}
}

void UInventory::LoadData(FInventoryData InInventoryData, AActor* InOwner)
{
	Initialize(InOwner, InInventoryData.SplitInfos);
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		Slots[i]->SetItem(InInventoryData.Items.IsValidIndex(i) ? InInventoryData.Items[i] : FItem::Empty);
	}
}

FInventoryData UInventory::ToData(bool bSaved)
{
	auto data = FInventoryData();

	data.bSaved = bSaved;
	data.SplitInfos = SplitInfos;

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		data.Items.Add(Slots[i]->GetItem());
	}
	return data;
}

void UInventory::AdditionItems(FItem& InItem, int32 InStartIndex, int32 InEndIndex)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Addition, tmpItem, InStartIndex, InEndIndex);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->AddItem(InItem);
		if (InItem.Count <= 0) break;
	}
}

void UInventory::AdditionItems(FItem& InItem, ESplitSlotType InSplitSlotType)
{
	const FSplitSlotInfo SplitSlotInfo = GetSplitSlotInfo(InSplitSlotType);
	AdditionItems(InItem, SplitSlotInfo.StartIndex, SplitSlotInfo.StartIndex + SplitSlotInfo.TotalCount);
}

void UInventory::RemoveItems(FItem& InItem, int32 InStartIndex, int32 InEndIndex)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Remove, tmpItem, InStartIndex, InEndIndex);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->SubItem(InItem);
		if (InItem.Count <= 0) break;
	}
}

void UInventory::RemoveItems(FItem& InItem, ESplitSlotType InSplitSlotType)
{
	const FSplitSlotInfo SplitSlotInfo = GetSplitSlotInfo(InSplitSlotType);
	RemoveItems(InItem, SplitSlotInfo.StartIndex, SplitSlotInfo.StartIndex + SplitSlotInfo.TotalCount);
}

void UInventory::ClearItems(FItem& InItem)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Clear, tmpItem);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->ClearItem();
	}
}

void UInventory::DiscardAll()
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		Slots[i]->DiscardItem();
	}
}

void UInventory::ClearAll()
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		Slots[i]->ClearItem();
	}
}

TArray<UInventorySlot*> UInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int32 InStartIndex, int32 InEndIndex)
{
	auto tmpList = TArray<UInventorySlot*>();
	if (InItem.Count <= 0) return tmpList;
	InStartIndex = FMath::Clamp(InStartIndex, 0, Slots.Num() - 1);
	if(InEndIndex != -1) InEndIndex = FMath::Clamp(InEndIndex, InStartIndex, Slots.Num() - 1);
	switch (InActionType)
	{
		case EInventoryActionType::Addition:
		{
			if (Slots[InStartIndex]->CanPutIn(InItem))
			{
				if (!tmpList.Contains(Slots[InStartIndex]))
				{
					tmpList.Add(Slots[InStartIndex]);
					InItem.Count -= Slots[InStartIndex]->GetRemainVolume();
					if (InItem.Count <= 0) return tmpList;
				}
			}
			for (int32 i = (InEndIndex == -1 ? 0 : InStartIndex); i < (InEndIndex == -1 ? Slots.Num() : InEndIndex); i++)
			{
				if (Slots[i]->Contains(InItem) && Slots[i]->CanPutIn(InItem))
				{
					if (!tmpList.Contains(Slots[i]))
					{
						tmpList.Add(Slots[i]);
						InItem.Count -= Slots[i]->GetRemainVolume();
						if (InItem.Count <= 0) return tmpList;
					}
				}
			}
			for (int32 i = (InEndIndex == -1 ? 0 : InStartIndex); i < (InEndIndex == -1 ? Slots.Num() : InEndIndex); i++)
			{
				if (Slots[i]->CanPutIn(InItem))
				{
					if (!tmpList.Contains(Slots[i]))
					{
						tmpList.Add(Slots[i]);
						InItem.Count -= Slots[i]->GetRemainVolume();
						if (InItem.Count <= 0) return tmpList;
					}
				}
			}
			break;
		}
		case EInventoryActionType::Remove:
		{
			if (Slots[InStartIndex]->Contains(InItem))
			{
				if (!tmpList.Contains(Slots[InStartIndex]))
				{
					tmpList.Add(Slots[InStartIndex]);
					InItem.Count -= Slots[InStartIndex]->GetItem().Count;
					if (InItem.Count <= 0) return tmpList;
				}
			}
			for (int32 i = (InEndIndex == -1 ? 0 : InStartIndex); i < (InEndIndex == -1 ? Slots.Num() : InEndIndex); i++)
			{
				if (Slots[i]->Contains(InItem))
				{
					if (!tmpList.Contains(Slots[i]))
					{
						tmpList.Add(Slots[i]);
						InItem.Count -= Slots[i]->GetItem().Count;
						if (InItem.Count <= 0) return tmpList;
					}
				}
			}
			break;
		}
		case EInventoryActionType::Clear:
		{
			for (int32 i = (InEndIndex == -1 ? 0 : InStartIndex); i < (InEndIndex == -1 ? Slots.Num() : InEndIndex); i++)
			{
				if (Slots[i]->Contains(InItem))
				{
					if (!tmpList.Contains(Slots[i]))
						tmpList.Add(Slots[i]);
				}
			}
			break;
		}
	}
	return tmpList;
}

TArray<UInventorySlot*> UInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, ESplitSlotType InSplitSlotType)
{
	const FSplitSlotInfo SplitSlotInfo = GetSplitSlotInfo(InSplitSlotType);
	return GetValidatedList(InActionType, InItem, SplitSlotInfo.StartIndex, SplitSlotInfo.StartIndex + SplitSlotInfo.TotalCount);
}

bool UInventory::HasSplitSlotInfo(ESplitSlotType InSplitSlotType) const
{
	return SplitInfos.Contains(InSplitSlotType);
}

FSplitSlotInfo UInventory::GetSplitSlotInfo(ESplitSlotType InSplitSlotType) const
{
	if(HasSplitSlotInfo(InSplitSlotType))
	{
		return SplitInfos[InSplitSlotType];
	}
	return FSplitSlotInfo();
}

TArray<UInventorySlot*> UInventory::K2_GetSplitSlots(ESplitSlotType InSplitSlotType)
{
	TArray<UInventorySlot*> SplitSlots = TArray<UInventorySlot*>();

	for (auto Iter : SplitInfos)
	{
		if (Iter.Key == InSplitSlotType)
		{
			for (int32 i = Iter.Value.StartIndex; i < Iter.Value.StartIndex + Iter.Value.TotalCount; i++)
			{
				SplitSlots.Add(Slots[i]);
			}
			break;
		}
	}

	return SplitSlots;
}

TMap<ESplitSlotType, FSplitSlotData> UInventory::GetSplitSlotDatas()
{
	TMap<ESplitSlotType, FSplitSlotData> SplitSlotDatas = TMap<ESplitSlotType, FSplitSlotData>();

	for (auto Iter : SplitInfos)
	{
		FSplitSlotData SplitSlotData = FSplitSlotData();
		for (int32 i = Iter.Value.StartIndex; i < Iter.Value.StartIndex + Iter.Value.TotalCount; i++)
		{
			SplitSlotData.Slots.Add(Slots[i]);
		}
		SplitSlotDatas.Add(Iter.Key, SplitSlotData);
	}

	return SplitSlotDatas;
}
