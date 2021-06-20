// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetPrimaryPanel.h"

UWidgetPrimaryPanel::UWidgetPrimaryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("PrimaryPanel");
	WidgetType = EWidgetPanelType::Permanent;
	InputMode = EInputMode::GameOnly;
}

void UWidgetPrimaryPanel::SetOwnerCharacter(ADWPlayerCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
}
