// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory/Inventory.h"
#include "CharacterInventory.generated.h"

class UInventoryEquipSlot;
class UInventorySkillSlot;

/**
 * ��ɫ��Ʒ��
 */
UCLASS()
class DREAMWORLD_API UCharacterInventory : public UInventory
{
	GENERATED_BODY()

public:
	UCharacterInventory();

public:
	virtual void Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos) override;
	
	virtual void LoadData(FInventoryData InInventoryData, AActor* InOwner) override;

	virtual FInventoryData ToData(bool bSaved = true) override;

	virtual void DiscardAll() override;

	virtual void ClearAll() override;

	virtual TArray<UInventorySlot*> GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int32 InStartIndex = 0, int32 InEndIndex = -1) override;

public:
	UFUNCTION(BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const;
};
