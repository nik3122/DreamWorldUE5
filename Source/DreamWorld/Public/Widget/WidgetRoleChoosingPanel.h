// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetRoleChoosingPanel.generated.h"

/**
 * ��ɫѡ�����
 */
UCLASS()
class DREAMWORLD_API UWidgetRoleChoosingPanel : public UWidgetPanelBase
{
	GENERATED_BODY()

public:
	UWidgetRoleChoosingPanel(const FObjectInitializer& ObjectInitializer);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RefreshList();

	void OnShowPanel() override;
};
