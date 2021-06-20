// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Inventory.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Inventory/InventorySlot.h"
#include "Vitality/Vitality.h"
#include "Abilities/Item/DWItemAbility.h"
#include "InventoryEquipSlot.h"
#include "InventorySkillSlot.h"

UInventory::UInventory()
{
	OwnerActor = nullptr;
	ConnectInventory = nullptr;
	Slots = TArray<UInventorySlot*>();
}

void UInventory::Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos)
{
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
				case ESplitSlotType::Shortcut:
				case ESplitSlotType::Auxiliary:
				case ESplitSlotType::Generate:
				{
					Slot = NewObject<UInventorySlot>(OwnerActor);
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

void UInventory::LoadData(FInventoryData InInventoryData, AActor* InOwner)
{
	Initialize(InOwner, InInventoryData.SplitInfos);
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		Slots[i]->SetItem(InInventoryData.Items[i]);
		IVitality* tmpVitality = Cast<IVitality>(OwnerActor);
		if (tmpVitality && InInventoryData.Items[i].GetData().AbilityClass)
		{
			Slots[i]->SetAbilityHandle(tmpVitality->AcquireAbility(InInventoryData.Items[i].GetData().AbilityClass));
		}
	}
}

FInventoryData UInventory::ToData()
{
	auto data = FInventoryData();

	data.bInitialized = true;
	data.SplitInfos = SplitInfos;

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		data.Items.Add(Slots[i]->GetItem());
	}
	return data;
}

void UInventory::AdditionItems(FItem& InItem, int InStartIndex)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Addition, tmpItem, InStartIndex);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->AddItem(InItem);
		if (InItem.Count <= 0) break;
	}
}

void UInventory::RemoveItems(FItem& InItem, int InStartIndex)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Remove, tmpItem, InStartIndex);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->SubItem(InItem);
		if (InItem.Count <= 0) break;
	}
}

void UInventory::ClearItems(FItem& InItem)
{
	FItem tmpItem = FItem::Clone(InItem);
	auto tmpList = GetValidatedList(EInventoryActionType::Clear, tmpItem);
	for (int i = 0; i < tmpList.Num(); i++)
	{
		tmpList[i]->SetItem(FItem::Empty);
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

TArray<UInventorySlot*> UInventory::GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int InStartIndex)
{
	auto tmpList = TArray<UInventorySlot*>();
	if (InItem.Count <= 0) return tmpList;
	InStartIndex = FMath::Clamp(InStartIndex, 0, Slots.Num() - 1);
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
			for (int i = 0; i < Slots.Num(); i++)
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
			for (int i = 0; i < Slots.Num(); i++)
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
			for (int i = 0; i < Slots.Num(); i++)
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
			for (int i = 0; i < Slots.Num(); i++)
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

FSplitSlotInfo UInventory::GetSplitSlotInfo(ESplitSlotType InSplitSlotType)
{
	if(SplitInfos.Contains(InSplitSlotType))
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
