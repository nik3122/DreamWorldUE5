// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/Slot/InventorySkillSlot.h"
#include "Inventory/Inventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "World/WorldManager.h"
#include "World/Chunk.h"
#include "DWGameInstance.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/Slot/WidgetInventorySkillSlot.h"
#include "Inventory/Character/CharacterInventory.h"

UInventorySkillSlot::UInventorySkillSlot()
{
	LimitType = EItemType::Skill;
}

void UInventorySkillSlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Super::InitSlot(InOwner, InItem, InLimitType, InSplitType);
}

bool UInventorySkillSlot::Active()
{
	if (Cast<ADWCharacter>(Owner->GetOwnerActor())->SkillAttack(Item.ID))
	{
		if(UISlot) Cast<UWidgetInventorySkillSlot>(UISlot)->StartCooldown();
		return true;
	}
	return false;
}

bool UInventorySkillSlot::UnActive()
{
	if (Cast<ADWCharacter>(Owner->GetOwnerActor())->GetSkillAbility(Item.ID).bCancelAble)
	{
		if(UISlot) Cast<UWidgetInventorySkillSlot>(UISlot)->StopCooldown();
		return true;
	}
	return false;
}

FCharacterSkillAbilityData UInventorySkillSlot::GetSkillData()
{
	return Cast<ADWCharacter>(Owner->GetOwnerActor())->GetSkillAbility(Item.ID);
}
