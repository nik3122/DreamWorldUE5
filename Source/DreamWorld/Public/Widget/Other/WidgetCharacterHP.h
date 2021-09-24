// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterHP.generated.h"

/**
 * ½ÇÉ«ÑªÌõ
 */
UCLASS()
class DREAMWORLD_API UWidgetCharacterHP : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidgetCharacterHP(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHeadInfo(const FString& InHeadInfo);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHealthPercent(float InHealth, float InMaxHealth);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetManaPercent(float InMana, float InMaxMana);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetStaminaPercent(float InStamina, float InMaxStamina);

	ADWCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
	
	void SetOwnerCharacter(ADWCharacter* val) { OwnerCharacter = val; }
};
