// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory/Inventory.h"
#include "CharacterInventory.generated.h"

class UInventoryEquipSlot;
class UInventorySkillSlot;

/**
 * ½ÇÉ«ÎïÆ·À¸
 */
UCLASS()
class DREAMWORLD_API UCharacterInventory : public UInventory
{
	GENERATED_BODY()

public:
	UCharacterInventory();

public:
	virtual void Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos) override;
	
	virtual void LoadData(FInventoryData InInventoryData, AActor* InOwner);

	virtual FInventoryData ToData();

	virtual void DiscardAll() override;

	virtual void ClearAll() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const;
};
