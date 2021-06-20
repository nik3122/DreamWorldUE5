// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "InventorySlot.generated.h"

class UInventory;

/**
 * 物品槽
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UInventorySlot : public UObject
{
	GENERATED_BODY()

public:
	UInventorySlot();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	FItem Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UInventory* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UWidgetInventorySlot* UISlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	EItemType LimitType;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	ESplitSlotType SplitType;

	FGameplayAbilitySpecHandle AbilityHandle;

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitSlot(UInventory* InOwner, FItem InItem, EItemType InLimitType = EItemType::None, ESplitSlotType InSplitType = ESplitSlotType::Default);
		
	UFUNCTION(BlueprintCallable)
	virtual void SetUISlot(UWidgetInventorySlot* InUISlot);

	UFUNCTION(BlueprintCallable)
	virtual bool CheckSlot(FItem InItem);

	UFUNCTION(BlueprintCallable)
	virtual bool CanPutIn(FItem InItem);
					
	UFUNCTION(BlueprintCallable)
	virtual bool Contains(FItem InItem);

	UFUNCTION(BlueprintCallable)
	virtual void Refresh();
	
	UFUNCTION(BlueprintCallable)
	virtual void SetItem(FItem& InItem, bool bReplace = false);

	UFUNCTION(BlueprintCallable)
	virtual void AddItem(FItem& InItem);

	UFUNCTION(BlueprintCallable)
	virtual void SubItem(FItem& InItem);
			
	UFUNCTION(BlueprintCallable)
	virtual void SplitItem(int InCount = -1);
			
	UFUNCTION(BlueprintCallable)
	virtual void MoveItem(int InCount = -1);

	UFUNCTION(BlueprintCallable)
	virtual void UseItem(int InCount = -1);

	UFUNCTION(BlueprintCallable)
	virtual void DiscardItem(int InCount = -1);

	UFUNCTION(BlueprintCallable)
	virtual void ClearItem();

public:
	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable)
	int GetRemainVolume() const;

	UFUNCTION(BlueprintCallable)
	int GetMaxVolume() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItem& GetItem()  { return Item; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventory* GetOwner() const { return Owner; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWidgetInventorySlot* GetUISlot() const { return UISlot; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EItemType GetLimitType() const { return LimitType; }

	UFUNCTION(BlueprintCallable)
	void SetLimitType(EItemType val) { LimitType = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayAbilitySpecHandle& GetAbilityHandle() { return AbilityHandle; }

	UFUNCTION(BlueprintCallable)
	void SetAbilityHandle(FGameplayAbilitySpecHandle val) { AbilityHandle = val; }
};
