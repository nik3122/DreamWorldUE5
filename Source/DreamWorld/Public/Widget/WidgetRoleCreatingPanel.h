// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Widget/WidgetPanelBase.h"
#include "WidgetRoleCreatingPanel.generated.h"

/**
 * ��ɫ�������
 */
UCLASS()
class DREAMWORLD_API UWidgetRoleCreatingPanel : public UWidgetPanelBase
{
	GENERATED_BODY()
	
public:
	UWidgetRoleCreatingPanel(const FObjectInitializer& ObjectInitializer);

public:
	void RefreshPanel() override; 

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetData();
};
