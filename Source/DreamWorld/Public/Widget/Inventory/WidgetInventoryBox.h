// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/Inventory/WidgetInventory.h"
#include "WidgetInventoryBox.generated.h"

class UWidgetInventorySlot;

/**
 * ÎïÆ·¿ò
 */
UCLASS()
class DREAMWORLD_API UWidgetInventoryBox : public UWidgetInventory
{
	GENERATED_BODY()

public:
	UWidgetInventoryBox(const FObjectInitializer& ObjectInitializer);
};
