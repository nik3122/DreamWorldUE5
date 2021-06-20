#include "Widget/Inventory/WidgetInventoryBase.h"
#include "Widget/Inventory/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventoryBar.h"

UWidgetInventoryBase::UWidgetInventoryBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UISlots = TArray<UWidgetInventorySlot*>();
	Inventory = nullptr;
}
