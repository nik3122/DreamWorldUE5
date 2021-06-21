// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventorySlot.generated.h"

class UWidgetInventory;
class UInventorySlot;

/**
 * UI物品槽
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UWidgetInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidgetInventorySlot(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UInventorySlot* OwnerSlot;

protected:
	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitSlot(UInventorySlot* InOwnerSlot);

	UFUNCTION(BlueprintCallable)
	bool IsEmpty();
		
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Refresh();
		
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetBorderColor(FLinearColor InColor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItem& GetItem();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventorySlot* GetSlot() const { return OwnerSlot; }
};
