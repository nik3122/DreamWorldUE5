// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WidgetInventorySkillSlot.h"

#include "TextBlock.h"
#include "Character/DWCharacter.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Inventory/Slot/InventorySkillSlot.h"
#include "Abilities/Character/DWCharacterSkillAbility.h"
#include "Kismet/KismetTextLibrary.h"
#include "Public/Inventory/CharacterInventory.h"

UWidgetInventorySkillSlot::UWidgetInventorySkillSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UWidgetInventorySkillSlot::InitSlot(UInventorySlot* InOwnerSlot)
{
	Super::InitSlot(InOwnerSlot);
}

void UWidgetInventorySkillSlot::UseItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->ActiveItem();
	}
}

void UWidgetInventorySkillSlot::Refresh()
{
	Super::Refresh();

	if(!IsEmpty())
	{
		TxtName->SetVisibility(ESlateVisibility::Visible);
		TxtName->SetText(GetItem().GetData().Name);

		TxtCost->SetVisibility(ESlateVisibility::Visible);
		if(OwnerSlot)
		{
			TxtCost->SetText(FText::FromString(FString::FromInt(FMath::Abs(OwnerSlot->GetAbilityInfo().Cost))));
			switch (OwnerSlot->GetAbilityInfo().CostType)
			{
				case EAbilityCostType::Health:
				{
					TxtCost->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));
					break;
				}
				case EAbilityCostType::Mana:
				{
					TxtCost->SetColorAndOpacity(FLinearColor(0.f, 0.65f, 1.f, 1.f));
					break;
				}
				case EAbilityCostType::Stamina:
				{
					TxtCost->SetColorAndOpacity(FLinearColor(0.f, 0.7f, 0.04f, 1.f));
					break;
				}
				default: break;
			}
		}
	}
	else
	{
		TxtName->SetVisibility(ESlateVisibility::Hidden);
		TxtCost->SetVisibility(ESlateVisibility::Hidden);
	}
}
