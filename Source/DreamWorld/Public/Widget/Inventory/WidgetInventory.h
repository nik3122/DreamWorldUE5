// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetInventory.generated.h"

class UWidgetInventorySlot;
class UWidgetInventoryEquipSlot;

/**
 * ��Ʒ������
 */
UCLASS()
class DREAMWORLD_API UWidgetInventory : public UWidgetPanelBase
{
	GENERATED_BODY()

public:
	UWidgetInventory(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UInventory* Inventory;

public:
	virtual void RefreshPanel() override;
	
	UFUNCTION(BlueprintCallable)
	void InitInventory(UInventory* InInventory);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitInventory"))
	void K2_InitInventory(UInventory* InInventory);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventory* GetInventory() const { return Inventory; }
	
	AActor* GetOwnerActor() const override;
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWidgetInventorySlot*> GetSplitUISlots(ESplitSlotType InSplitSlotType);
};