// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventorySkillSlot.h"
#include "Widget/Inventory/WidgetInventoryBase.h"
#include "DragDropOperation.h"
#include "Character/DWCharacter.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "DWGameMode.h"
#include "Inventory/InventorySlot.h"
#include "Inventory/InventorySkillSlot.h"
#include "Abilities/Character/DWCharacterSkillAbility.h"
#include "Public/Inventory/CharacterInventory.h"

UWidgetInventorySkillSlot::UWidgetInventorySkillSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OwnerSlot = nullptr;
	AbilityInfo = FDWAbilityInfo();
}

void UWidgetInventorySkillSlot::InitSlot(UInventorySlot* InOwnerSlot)
{
	Super::InitSlot(InOwnerSlot);

	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerSlot->GetOwner()->GetOwnerActor());
	if (OwnerCharacter && !OwnerSlot->IsEmpty())
	{
		//OwnerCharacter->GetAbilityInfo(Cast<UInventorySkillSlot>(OwnerSlot)->GetSkillData().AbilityClass, AbilityInfo);
	}
}

bool UWidgetInventorySkillSlot::Active()
{
	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerSlot->GetOwner()->GetOwnerActor());
	if (OwnerCharacter && !OwnerSlot->IsEmpty())
	{
		if (Cast<UInventorySkillSlot>(OwnerSlot)->Active())
		{
			return true;
		}
	}
	return false;
}

bool UWidgetInventorySkillSlot::UnActive()
{
	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerSlot->GetOwner()->GetOwnerActor());
	if (OwnerCharacter && !OwnerSlot->IsEmpty())
	{
		if (Cast<UInventorySkillSlot>(OwnerSlot)->UnActive())
		{
			return true;
		}
	}
	return false;
}
