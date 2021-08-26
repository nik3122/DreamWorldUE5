#include "Widget/Inventory/WidgetInventory.h"

#include "Inventory.h"
#include "InventorySlot.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"

UWidgetInventory::UWidgetInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Inventory = nullptr;
}

void UWidgetInventory::RefreshPanel()
{
	Super::RefreshPanel();
	if (!Inventory)
	{
		SetActive(false);
	}
}

void UWidgetInventory::InitInventory(UInventory* InInventory)
{
	Inventory = InInventory;
	K2_InitInventory(Inventory);
	RefreshPanel();
}

AActor* UWidgetInventory::GetOwnerActor() const
{
	if(Inventory) return Inventory->GetOwnerActor();
	return nullptr;
}

TArray<UWidgetInventorySlot*> UWidgetInventory::GetSplitUISlots(ESplitSlotType InSplitSlotType)
{
	TArray<UWidgetInventorySlot*> UISlots = TArray<UWidgetInventorySlot*>();
	if(Inventory)
	{
		TArray<UInventorySlot*> Slots = Inventory->GetSplitSlots<UInventorySlot>(InSplitSlotType);
		for (int32 i = 0; i < Slots.Num(); i++)
		{
			if(Slots[i]->GetUISlot())
			{
				UISlots.Add(Slots[i]->GetUISlot());
			}
		}
	}
	return UISlots;
}
