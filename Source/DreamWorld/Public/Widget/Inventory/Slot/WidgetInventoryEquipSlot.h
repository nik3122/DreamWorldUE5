// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "WidgetInventoryEquipSlot.generated.h"

/**
 * UI×°±¸²Û
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UWidgetInventoryEquipSlot : public UWidgetInventorySlot
{
	GENERATED_BODY()

public:
	UWidgetInventoryEquipSlot(const FObjectInitializer& ObjectInitializer);

public:
	virtual void InitSlot(UInventorySlot* InOwnerSlot) override;
};
