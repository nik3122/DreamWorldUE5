// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/Inventory/WidgetInventory.h"
#include "WidgetInventoryPanel.generated.h"

class UWidgetInventoryEquipSlot;

/**
 * ��ɫ���
 */
UCLASS()
class DREAMWORLD_API UWidgetInventoryPanel : public UWidgetInventory
{
	GENERATED_BODY()

public:
	UWidgetInventoryPanel(const FObjectInitializer& ObjectInitializer);

protected:
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHeadInfo(const FString& InHeadInfo);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHealthInfo(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetManaInfo(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetStaminaInfo(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAttackForce(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAttackSpeed(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAttackCritRate(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetAttackStealRate(const FString& InValue);
			
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetRepulseForce(const FString& InValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDefendRate(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetPhysicsDefRate(const FString& InValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetMagicDefRate(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetToughnessRate(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetMoveSpeed(const FString& InValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetSwimSpeed(const FString& InValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetRideSpeed(const FString& InValue);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetFlySpeed(const FString& InValue);
};
