// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Slot/InventorySkillSlot.h"
#include "Inventory/Inventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/Slot/WidgetInventorySkillSlot.h"

UInventorySkillSlot::UInventorySkillSlot()
{
	LimitType = EItemType::Skill;
}

void UInventorySkillSlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Super::InitSlot(InOwner, InItem, InLimitType, InSplitType);
}

void UInventorySkillSlot::PreSet(FItem& InItem)
{
	Super::PreSet(InItem);
	if(GetSkillData().GetItemData().SkillMode == ESkillMode::Passive)
	{
		CancelItem();
	}
}

void UInventorySkillSlot::EndSet()
{
	Super::EndSet();
	if(GetSkillData().GetItemData().SkillMode == ESkillMode::Passive)
	{
		ActiveItem();
	}
}

bool UInventorySkillSlot::ActiveItem()
{
	Super::ActiveItem();
	if(ADWCharacter* Character = Cast<ADWCharacter>(Owner->GetOwnerActor()))
	{
		if (Character->SkillAttack(Item.ID))
		{
			if(UISlot && GetSkillData().GetItemData().SkillMode == ESkillMode::Initiative)
			{
				Cast<UWidgetInventorySkillSlot>(UISlot)->StartCooldown();
			}
			return true;
		}
	}
	return false;
}

bool UInventorySkillSlot::CancelItem()
{
	if (GetSkillData().bCancelAble)
	{
		Super::CancelItem();
		if(UISlot && GetSkillData().GetItemData().SkillMode == ESkillMode::Initiative)
		{
			Cast<UWidgetInventorySkillSlot>(UISlot)->StopCooldown();
		}
		return true;
	}
	return false;
}

FDWCharacterSkillAbilityData UInventorySkillSlot::GetSkillData() const
{
	if(ADWCharacter* Character = Cast<ADWCharacter>(Owner->GetOwnerActor()))
	{
		return Character->GetSkillAbility(Item.ID);
	}
	return FDWCharacterSkillAbilityData();
}
