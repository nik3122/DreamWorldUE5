// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "UserWidgetBase.h"
#include "WidgetRoleCreatingPanel.generated.h"

/**
 * ��ɫ�������
 */
UCLASS()
class DREAMWORLD_API UWidgetRoleCreatingPanel : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UWidgetRoleCreatingPanel(const FObjectInitializer& ObjectInitializer);

public:
	virtual void OnRefresh_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ResetData();
};
