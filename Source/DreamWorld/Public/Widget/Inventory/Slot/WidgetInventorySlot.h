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
	virtual void NativePreConstruct() override;
	
	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitSlot(UInventorySlot* InOwnerSlot);

	UFUNCTION(BlueprintCallable)
	virtual void SplitItem(int InCount = -1);
	
	UFUNCTION(BlueprintCallable)
	virtual void MoveItem(int InCount = -1);

	UFUNCTION(BlueprintCallable)
	virtual void UseItem(int InCount = -1);

	UFUNCTION(BlueprintCallable)
	virtual void DiscardItem(int InCount = -1);

public:
	UFUNCTION(BlueprintCallable)
	virtual void Refresh();
				
	UFUNCTION(BlueprintCallable)
	void RefreshCooldown();
	
	UFUNCTION(BlueprintCallable)
	void SetBorderColor(FLinearColor InColor);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	UInventorySlot* OwnerSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UBorder* Border;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UCanvasPanel* Content;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UImage* ImgIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UImage* ImgMask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UTextBlock* TxtCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UTextBlock* TxtCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (BindWidget, OptionalWidget = false))
	class UTextBlock* TxtKeyCode;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText KeyCode;

	UPROPERTY()
	UMaterialInstanceDynamic* MaskMatInst;

public:
	UFUNCTION(BlueprintPure)
	bool IsEmpty() const;

	UFUNCTION(BlueprintPure)
	FItem& GetItem() const;

	UFUNCTION(BlueprintPure)
	UInventorySlot* GetOwnerSlot() const { return OwnerSlot; }
};
