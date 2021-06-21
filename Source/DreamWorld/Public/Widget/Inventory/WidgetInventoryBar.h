// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/Inventory/WidgetInventory.h"
#include "WidgetInventoryBar.generated.h"

class UWidgetInventorySkillSlot;
class ADWPlayerCharacter;

/**
 * ��Ʒ��
 */
UCLASS()
class DREAMWORLD_API UWidgetInventoryBar : public UWidgetInventory
{
	GENERATED_BODY()

public:
	UWidgetInventoryBar(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int32 SelectedSlotIndex;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateSelectBox();

	UFUNCTION(BlueprintCallable)
	void PrevInventorySlot();

	UFUNCTION(BlueprintCallable)
	void NextInventorySlot();
	
	UFUNCTION(BlueprintCallable)
	void SelectInventorySlot(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventorySlot* GetSelectedSlot();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItem GetSelectedItem();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }
};
