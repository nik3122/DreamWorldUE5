// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetPrimaryPanel.generated.h"

class ADWPlayerCharacter;

/**
 * 主界面
 */
UCLASS()
class DREAMWORLD_API UWidgetPrimaryPanel : public UWidgetPanelBase
{
	GENERATED_BODY()

public:
	UWidgetPrimaryPanel(const FObjectInitializer& ObjectInitializer);

public:
	AActor* GetOwnerActor() const override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHeadInfo(const FString& InHeadInfo);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHealthPercent(float InHealth, float InMaxHealth);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetManaPercent(float InMana, float InMaxMana);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetStaminaPercent(float InStamina, float InMaxStamina);
};
