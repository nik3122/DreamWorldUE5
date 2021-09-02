// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetPrimaryPanel.h"

#include "DWPlayerCharacter.h"

UWidgetPrimaryPanel::UWidgetPrimaryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("PrimaryPanel");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameOnly;
}

AActor* UWidgetPrimaryPanel::GetOwnerActor() const
{
	return UDWHelper::GetPlayerCharacter(this);
}
