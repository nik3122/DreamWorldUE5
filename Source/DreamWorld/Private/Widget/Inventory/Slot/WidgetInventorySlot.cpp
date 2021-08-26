// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventory.h"
#include "DragDropOperation.h"
#include "Inventory/Slot/InventorySlot.h"

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

FReply UWidgetInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UWidgetInventorySlot::InitSlot(UInventorySlot* InOwnerSlot)
{
	OwnerSlot = InOwnerSlot;
	if(OwnerSlot)
	{
		OwnerSlot->SetUISlot(this);
	}
}

void UWidgetInventorySlot::SplitItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->SplitItem(InCount);
	}
}

void UWidgetInventorySlot::MoveItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->MoveItem(InCount);
	}
}

void UWidgetInventorySlot::UseItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->UseItem(InCount);
	}
}

void UWidgetInventorySlot::DiscardItem(int InCount)
{
	if(OwnerSlot)
	{
		OwnerSlot->DiscardItem(InCount);
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
