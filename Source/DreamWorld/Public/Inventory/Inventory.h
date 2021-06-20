// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Inventory.generated.h"

class UInventorySlot;

/**
 * ��Ʒ��
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UInventory : public UObject
{
	GENERATED_BODY()

public:
	UInventory();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<UInventorySlot*> Slots;
				
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	TMap<ESplitSlotType, FSplitSlotInfo> SplitInfos;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	AActor* OwnerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UInventory* ConnectInventory;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Initialize(AActor* InOwner, TMap<ESplitSlotType, FSplitSlotInfo> InSplitInfos);

	UFUNCTION(BlueprintCallable)
	virtual void LoadData(FInventoryData InInventoryData, AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	virtual FInventoryData ToData();

	UFUNCTION(BlueprintCallable)
	virtual void AdditionItems(FItem& InItem, int InStartIndex = 0);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveItems(FItem& InItem, int InStartIndex = 0);

	UFUNCTION(BlueprintCallable)
	virtual void ClearItems(FItem& InItem);

	UFUNCTION(BlueprintCallable)
	virtual void DiscardAll();

	UFUNCTION(BlueprintCallable)
	virtual void ClearAll();

	UFUNCTION(BlueprintCallable)
	virtual TArray<UInventorySlot*> GetValidatedList(EInventoryActionType InActionType, FItem& InItem, int InStartIndex = 0);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetOwnerActor() const { return OwnerActor; }
	
	UFUNCTION(BlueprintCallable)
	void SetOwnerActor(AActor* val) { OwnerActor = val; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventory* GetConnectInventory() const { return ConnectInventory; }

	UFUNCTION(BlueprintCallable)
	void SetConnectInventory(UInventory* val) { ConnectInventory = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UInventorySlot*> GetSlots() const { return Slots; }
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FSplitSlotInfo GetSplitSlotInfo(ESplitSlotType InSplitSlotType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<ESplitSlotType, FSplitSlotInfo> GetSplitSlotInfos() const { return SplitInfos; }

	template<typename T>
	TArray<T*> GetSplitSlots(ESplitSlotType InSplitSlotType)
	{
		TArray<T*> SplitSlots = TArray<T*>();

		for (auto Iter : SplitInfos)
		{
			if(Iter.Key == InSplitSlotType)
			{
				for (int32 i = Iter.Value.StartIndex; i < Iter.Value.StartIndex + Iter.Value.TotalCount; i++)
				{
					SplitSlots.Add(Cast<T>(Slots[i]));
				}
				break;
			}
		}

		return SplitSlots;
	}
		
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetSplitSlots"))
	TArray<UInventorySlot*> K2_GetSplitSlots(ESplitSlotType InSplitSlotType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<ESplitSlotType, FSplitSlotData> GetSplitSlotDatas();
};
