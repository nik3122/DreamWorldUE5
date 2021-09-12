#include "Widget/Inventory/WidgetInventory.h"

#include "Inventory.h"
#include "InventorySlot.h"
#include "Vitality.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"

UWidgetInventory::UWidgetInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UWidgetInventory::OnRefresh_Implementation()
{
	Super::OnRefresh_Implementation();
}

UInventory* UWidgetInventory::GetInventory() const
{
	if(IVitality* Vitality = Cast<IVitality>(OwnerActor))
	{
		return Vitality->GetInventory();
	}
	return nullptr;
}

TArray<UWidgetInventorySlot*> UWidgetInventory::GetSplitUISlots(ESplitSlotType InSplitSlotType) const
{
	TArray<UWidgetInventorySlot*> UISlots = TArray<UWidgetInventorySlot*>();
	if(GetInventory())
	{
		TArray<UInventorySlot*> Slots = GetInventory()->GetSplitSlots<UInventorySlot>(InSplitSlotType);
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
