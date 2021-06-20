// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/Inventory/WidgetInventoryBase.h"
#include "WidgetInventoryBar.generated.h"

class UWidgetInventorySkillSlot;
class ADWPlayerCharacter;

/**
 * ŒÔ∆∑¿∏
 */
UCLASS()
class DREAMWORLD_API UWidgetInventoryBar : public UWidgetInventoryBase
{
	GENERATED_BODY()

public:
	UWidgetInventoryBar(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<UWidgetInventorySkillSlot*> UISkillSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int32 SelectedSlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	ADWPlayerCharacter* OwnerCharacter;

public:
	void SetActive(bool bActive) override;

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(ADWPlayerCharacter* InOwnerCharacter);

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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UWidgetInventorySkillSlot*> GetUISkillSlots() const { return UISkillSlots; }
};
