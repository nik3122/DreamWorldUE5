// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory/Character/CharacterInventory.h"
#include "PlayerInventory.generated.h"

/**
 * ÕÊº“ŒÔ∆∑¿∏
 */
UCLASS()
class DREAMWORLD_API UPlayerInventory : public UCharacterInventory
{
	GENERATED_BODY()

public:
	UPlayerInventory();

public:
	virtual void Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos) override;

	virtual TArray<UInventorySlot*> GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int InStartIndex = 0) override;
};
