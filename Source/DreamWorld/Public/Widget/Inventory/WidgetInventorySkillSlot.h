// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventorySlot.h"
#include "WidgetInventorySkillSlot.generated.h"

class UWidgetInventoryBase;

/**
 * UI技能槽
 */
UCLASS(BlueprintType)
class DREAMWORLD_API UWidgetInventorySkillSlot : public UWidgetInventorySlot
{
	GENERATED_BODY()
	
public:
	UWidgetInventorySkillSlot(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	FDWAbilityInfo AbilityInfo;

public:
	virtual void InitSlot(UInventorySlot* InOwnerSlot) override;
	
	UFUNCTION(BlueprintCallable)
	bool Active();
		
	UFUNCTION(BlueprintCallable)
	bool UnActive();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartCooldown();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StopCooldown();
};
