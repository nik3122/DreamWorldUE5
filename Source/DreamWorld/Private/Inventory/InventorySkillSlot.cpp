// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Inventory/InventorySkillSlot.h"
#include "Inventory/Inventory.h"
#include "Inventory/InventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "World/WorldManager.h"
#include "World/Chunk.h"
#include "DWGameInstance.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/WidgetInventorySkillSlot.h"
#include "Inventory/CharacterInventory.h"

UInventorySkillSlot::UInventorySkillSlot()
{
	SkillIndex = -1;
	LimitType = EItemType::Skill;
}

void UInventorySkillSlot::InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/)
{
	Super::InitSlot(InOwner, InItem, InLimitType, InSplitType);
	//SkillIndex = InSkillIndex;
}

void UInventorySkillSlot::Clear()
{
	if (IsEmpty()) return;

	SkillIndex = -1;
}

bool UInventorySkillSlot::Active()
{
	if (Cast<ADWCharacter>(Owner->GetOwnerActor())->SkillAttack(SkillIndex))
	{
		if(UISlot) Cast<UWidgetInventorySkillSlot>(UISlot)->StartCooldown();
		return true;
	}
	return false;
}

bool UInventorySkillSlot::UnActive()
{
	if (Cast<ADWCharacter>(Owner->GetOwnerActor())->GetSkillAbility(SkillIndex).bCancelAble)
	{
		if(UISlot) Cast<UWidgetInventorySkillSlot>(UISlot)->StopCooldown();
		return true;
	}
	return false;
}

FDWCharacterSkillAbilityData UInventorySkillSlot::GetSkillData()
{
	return Cast<ADWCharacter>(Owner->GetOwnerActor())->GetSkillAbility(SkillIndex);
}
