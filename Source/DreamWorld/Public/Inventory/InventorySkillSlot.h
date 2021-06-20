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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int32 SkillIndex;

public:
	virtual void InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType /* = EItemType::NoZne */, ESplitSlotType InSplitType /*= ESplitSlotType::Default*/) override;

	UFUNCTION(BlueprintCallable)
	void Clear();
	
	UFUNCTION(BlueprintCallable)
	bool Active();
		
	UFUNCTION(BlueprintCallable)
	bool UnActive();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDWCharacterSkillAbilityData GetSkillData();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSkillIndex() const { return SkillIndex; }
};
