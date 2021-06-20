// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetWorldChoosingPanel.generated.h"

/**
 * 世界选择面板
 */
UCLASS()
class DREAMWORLD_API UWidgetWorldChoosingPanel : public UWidgetPanelBase
{
	GENERATED_BODY()
	
public:
	UWidgetWorldChoosingPanel(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RefreshList();

	void OnShowPanel() override;
};
