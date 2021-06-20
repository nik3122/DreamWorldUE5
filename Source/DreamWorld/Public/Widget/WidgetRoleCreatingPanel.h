// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetRoleCreatingPanel.generated.h"

/**
 * 角色创建面板
 */
UCLASS()
class DREAMWORLD_API UWidgetRoleCreatingPanel : public UWidgetPanelBase
{
	GENERATED_BODY()
	
public:
	UWidgetRoleCreatingPanel(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetData();

	void OnShowPanel() override;
};
