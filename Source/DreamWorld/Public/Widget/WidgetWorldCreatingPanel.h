// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetWorldCreatingPanel.generated.h"

/**
 * 世界创建面板
 */
UCLASS()
class DREAMWORLD_API UWidgetWorldCreatingPanel : public UWidgetPanelBase
{
	GENERATED_BODY()
	
public:
	UWidgetWorldCreatingPanel(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetData();

	void OnShowPanel() override;
};
