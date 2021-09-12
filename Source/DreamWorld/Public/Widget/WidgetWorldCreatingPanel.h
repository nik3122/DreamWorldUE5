// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "UserWidgetBase.h"
#include "WidgetWorldCreatingPanel.generated.h"

/**
 * ���紴�����
 */
UCLASS()
class DREAMWORLD_API UWidgetWorldCreatingPanel : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UWidgetWorldCreatingPanel(const FObjectInitializer& ObjectInitializer);

public:
	virtual void OnRefresh_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetData();
};
