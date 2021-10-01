// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Slot/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "World/WorldManager.h"
#include "World/Chunk.h"
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
	return (IsEmpty() && CheckSlot(InItem)) || (Item.EqualType(InItem) && GetRemainVolume() > 0);
}

bool UInventorySlot::Contains(FItem InItem)
{
	return !IsEmpty() && Item.EqualType(InItem);
}

void UInventorySlot::Refresh()
{
	if (!IsEmpty() && Item.Count <= 0)
	{
		SetItem(FItem::Empty, false);
	}
	if (UISlot) UISlot->Refresh();
}

void UInventorySlot::PreSet(FItem& InItem)
{
	
}

void UInventorySlot::EndSet()
{
	if(Item.IsValid())
	{
		IVitality* vitality = Cast<IVitality>(Owner->GetOwnerActor());
		if (vitality && Item.GetData().AbilityClass)
		{
			AbilityHandle = vitality->AcquireAbility(Item.GetData().AbilityClass, Item.GetData().Level);
		}
	}
	else
	{
		AbilityHandle = FGameplayAbilitySpecHandle();
	}
}

void UInventorySlot::Replace(UInventorySlot* InSlot)
{
	auto tmpItem = GetItem();
	SetItem(InSlot->GetItem());
	InSlot->SetItem(tmpItem);

	// const auto abilityHandle = GetAbilityHandle();
	// SetAbilityHandle(InSlot->GetAbilityHandle());
	// InSlot->SetAbilityHandle(abilityHandle);

	const auto cooldownInfo = GetCooldownInfo();
	SetCooldownInfo(InSlot->GetCooldownInfo());
	InSlot->SetCooldownInfo(cooldownInfo);
}

void UInventorySlot::SetItem(FItem& InItem, bool bRefresh)
{
	PreSet(InItem);
	Item = InItem;
	Item.Count = FMath::Clamp(Item.Count, 0, GetMaxVolume());
	EndSet();
	if(bRefresh)
	{
		Refresh();
	}
}

void UInventorySlot::AddItem(FItem& InItem)
{
	if (Contains(InItem))
	{
		int tmpNum = InItem.Count - GetRemainVolume();
		Item.Count = FMath::Clamp(Item.Count + InItem.Count, 0, GetMaxVolume());
		InItem.Count = tmpNum;
		Refresh();
	}
	else
	{
		SetItem(InItem);
		InItem.Count -= GetMaxVolume();
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

	if(Owner->GetConnectInventory())
	{
		Owner->GetConnectInventory()->AdditionItems(tmpItem);
	}
	else
	{
		switch (Item.GetData().Type)
		{
			case EItemType::Voxel:
			case EItemType::Prop:
			{
				if(GetSplitType() != ESplitSlotType::Shortcut)
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Shortcut);
				}
				else
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Default);
				}
				break;
			}
			case EItemType::Equip:
			{
				if(GetSplitType() != ESplitSlotType::Equip)
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Equip);
				}
				else
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Default);
					Owner->AdditionItems(tmpItem, ESplitSlotType::Shortcut);
				}
				break;
			}
			case EItemType::Skill:
			{
				if(GetSplitType() != ESplitSlotType::Skill)
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Skill);
				}
				else
				{
					Owner->AdditionItems(tmpItem, ESplitSlotType::Default);
					Owner->AdditionItems(tmpItem, ESplitSlotType::Shortcut);
				}
				break;
			}
			default: break;
		}
	}

	tmpItem.Count = Item.Count - tmpItem.Count;
	SubItem(tmpItem);
}

void UInventorySlot::UseItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if (InCount == -1) InCount = Item.Count;
	
	switch (Item.GetData().Type)
	{
		case EItemType::Voxel:
		{
			if(ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(GetOwner()->GetOwnerActor()))
			{
				for(int32 i = 0; i < InCount; i ++)
				{
					FItem tmpItem = FItem::Clone(Item, 1);
					if(OwnerCharacter->OnUseItem(tmpItem))
					{
						SubItem(tmpItem);
					}
					else break;
				}
			}
			break;
		}
		case EItemType::Prop:
		{
			if(ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(GetOwner()->GetOwnerActor()))
			{
				for(int32 i = 0; i < InCount; i ++)
				{
					FItem tmpItem = FItem::Clone(Item, 1);
					if(OwnerCharacter->OnUseItem(tmpItem) && ActiveItem())
					{
						SubItem(tmpItem);
					}
					else break;
				}
			}
			break;
		}
		case EItemType::Equip:
		{
			MoveItem(InCount);
			break;
		}
		case EItemType::Skill:
		{
			MoveItem(InCount);
			break;
		}
		default: break;
	}
}

void UInventorySlot::DiscardItem(int InCount /*= -1*/)
{
	if (IsEmpty()) return;

	if (InCount == -1) InCount = Item.Count;
	FItem tmpItem = FItem::Clone(Item, InCount);
	auto chunk = AWorldManager::GetCurrent()->FindChunk(Owner->GetOwnerActor()->GetActorLocation());
	if (chunk != nullptr)
	{
		chunk->SpawnPickUp(tmpItem, Owner->GetOwnerActor()->GetActorLocation() + FMath::RandPointInBox(FBox(FVector(-20, -20, -10), FVector(20, 20, 10))));
	}
	SubItem(tmpItem);
}

bool UInventorySlot::ActiveItem()
{
	if(AbilityHandle.IsValid())
	{
		if(IVitality* Vitality = Cast<IVitality>(GetOwner()->GetOwnerActor()))
		{
			return Vitality->ActiveAbility(AbilityHandle);
		}
	}
    return false;
}

bool UInventorySlot::CancelItem()
{
	if(AbilityHandle.IsValid())
	{
		if(IVitality* Vitality = Cast<IVitality>(GetOwner()->GetOwnerActor()))
		{
			Vitality->CancelAbility(AbilityHandle);
			return true;
		}
	}
	return false;
}

void UInventorySlot::ClearItem()
{
	if (IsEmpty()) return;

	SetItem(FItem::Empty);
}

void UInventorySlot::StartCooldown()
{
	CooldownInfo.bCooldowning = true;
	CooldownInfo.TotalTime = GetAbilityInfo().Cooldown;
	CooldownInfo.RemainTime = CooldownInfo.TotalTime;
}

void UInventorySlot::StopCooldown()
{
	CooldownInfo.bCooldowning = false;
	CooldownInfo.RemainTime = 0.f;
}

void UInventorySlot::RefreshCooldown(float DeltaSeconds)
{
	if(CooldownInfo.bCooldowning)
	{
		CooldownInfo.RemainTime -= DeltaSeconds;
		if(CooldownInfo.RemainTime <= 0.f)
		{
			StopCooldown();
		}
	}
	if(UISlot)
	{
		UISlot->RefreshCooldown();
	}
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
	return Item.IsValid() ? Item.GetData().MaxCount : 0;
}

FDWAbilityInfo UInventorySlot::GetAbilityInfo() const
{
	FDWAbilityInfo AbilityInfo;
	if(ADWCharacter* Character = Cast<ADWCharacter>(Owner->GetOwnerActor()))
	{
		Character->GetAbilityInfo(Item.GetData().AbilityClass, AbilityInfo);
	}
	return AbilityInfo;
}
