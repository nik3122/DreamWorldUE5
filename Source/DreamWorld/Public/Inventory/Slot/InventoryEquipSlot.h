// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory/Slot/InventorySlot.h"
#include "InventoryEquipSlot.generated.h"

/**
 * ×°±¸²Û
 */
UCLASS()
class DREAMWORLD_API UInventoryEquipSlot : public UInventorySlot
{
	GENERATED_BODY()

public:
	UInventoryEquipSlot();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	EEquipPartType PartType;

public:
	virtual void InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/) override;

	virtual bool CheckSlot(FItem InItem) override;
	
	virtual void Refresh() override;

	virtual void UseItem(int InCount = -1) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEquipPartType GetPartType() const { return PartType; }

	UFUNCTION(BlueprintCallable)
	void SetPartType(EEquipPartType val) { PartType = val; }
};
