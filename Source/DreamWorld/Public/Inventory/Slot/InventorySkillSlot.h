// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "InventorySlot.h"
#include "InventorySkillSlot.generated.h"

class UCharacterInventory;

/**
 * ���ܲ�
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UInventorySkillSlot : public UInventorySlot
{
	GENERATED_BODY()

public:
	UInventorySkillSlot();

public:
	virtual void InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::None */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/) override;

	virtual void PreSet(FItem& InItem) override;

	virtual void EndSet() override;
	
	virtual bool ActiveItem() override;
	
	virtual bool CancelItem() override;

public:
	virtual FDWAbilityInfo GetAbilityInfo() const override;
	
	UFUNCTION(BlueprintPure)
	FDWCharacterSkillAbilityData GetSkillData() const;
};
