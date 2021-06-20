// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetInventoryBase.generated.h"

class UWidgetInventorySlot;
class UWidgetInventoryEquipSlot;

/**
 * 物品面板基类
 */
UCLASS()
class DREAMWORLD_API UWidgetInventoryBase : public UWidgetPanelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<UWidgetInventorySlot*> UISlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UInventory* Inventory;

public:
	UWidgetInventoryBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void InitInventory(UInventory* InInventory);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventory* GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWidgetInventorySlot*> GetUISlots() const { return UISlots; }
};