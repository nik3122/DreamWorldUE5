// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Blueprint/UserWidget.h"
#include "WidgetVitalityHP.generated.h"

class AVitalityObject;

/**
 * ÉúÃüÌåÑªÌõ
 */
UCLASS()
class DREAMWORLD_API UWidgetVitalityHP : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidgetVitalityHP(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	AVitalityObject* OwnerObject;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHeadInfo(const FString& InHeadInfo);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetHealthPercent(float InHealth, float InMaxHealth);

	AVitalityObject* GetOwnerObject() const { return OwnerObject; }

	void SetOwnerObject(AVitalityObject* val) { OwnerObject = val; }
};
