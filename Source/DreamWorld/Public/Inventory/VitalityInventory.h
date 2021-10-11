// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory/Inventory.h"
#include "VitalityInventory.generated.h"

/**
 * �����Ʒ��
 */
UCLASS()
class DREAMWORLD_API UVitalityInventory : public UInventory
{
	GENERATED_BODY()

public:
	UVitalityInventory();

public:
	virtual void Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos) override;
	
	virtual void LoadData(FInventorySaveData InInventoryData, AActor* InOwner) override;

	virtual FInventorySaveData ToData(bool bSaved = true) override;

	virtual void DiscardAll() override;

	virtual void ClearAll() override;

	virtual TArray<UInventorySlot*> GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int32 InStartIndex = 0, int32 InEndIndex = -1) override;

public:
	UFUNCTION(BlueprintPure)
	AVitalityObject* GetOwnerVitality() const;
};
