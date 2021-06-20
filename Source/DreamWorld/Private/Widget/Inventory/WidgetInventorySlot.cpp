// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBase.h"
#include "DragDropOperation.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "DWGameMode.h"
#include "Inventory/InventorySlot.h"

UWidgetInventorySlot::UWidgetInventorySlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OwnerSlot = nullptr;
}

bool UWidgetInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	auto payloadSlot = Cast<UWidgetInventorySlot>(InOperation->Payload);
	if (payloadSlot && payloadSlot != this && !payloadSlot->IsEmpty())
	{
		FItem& tmpItem = payloadSlot->GetItem();
		if(OwnerSlot->CheckSlot(tmpItem))
		{
			if (OwnerSlot->Contains(tmpItem))
			{
				OwnerSlot->AddItem(tmpItem);
			}
			else
			{
				auto abilityHandle = OwnerSlot->GetAbilityHandle();
				OwnerSlot->SetItem(tmpItem, true);
				OwnerSlot->SetAbilityHandle(payloadSlot->GetSlot()->GetAbilityHandle());
				payloadSlot->GetSlot()->SetAbilityHandle(abilityHandle);
			}
			payloadSlot->GetSlot()->Refresh();
		}
	}
	return true;
}

void UWidgetInventorySlot::InitSlot(UInventorySlot* InOwnerSlot)
{
	OwnerSlot = InOwnerSlot;
	if(OwnerSlot)
	{
		OwnerSlot->SetUISlot(this);
	}
}

bool UWidgetInventorySlot::IsEmpty()
{
	if (OwnerSlot) return OwnerSlot->GetItem() == FItem::Empty;
	return true;
}

FItem& UWidgetInventorySlot::GetItem()
{
	if(OwnerSlot) return OwnerSlot->GetItem();
	return FItem::Empty;
}
