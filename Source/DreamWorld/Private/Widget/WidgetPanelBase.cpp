// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetPanelBase.h"
#include "DWGameInstance.h"
#include "DWGameMode.h"

UWidgetPanelBase::UWidgetPanelBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = NAME_None;
	WidgetType = EWidgetPanelType::Temporary;
	InputMode = EInputMode::GameOnly;
}

void UWidgetPanelBase::SetActive(bool bActive)
{
	if (bActive)
	{
		if(!IsInViewport()) AddToViewport();
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if(WidgetType == EWidgetPanelType::Temporary)
		{
			UDWHelper::GetGameMode()->SetTemporaryPanel(this);
		}
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		if(WidgetType == EWidgetPanelType::Temporary)
		{
			if(IsInViewport()) RemoveFromViewport();
			UDWHelper::GetGameMode()->SetTemporaryPanel(nullptr);
		}
	}
	UDWHelper::GetGameMode()->UpdateInputMode();
	RefreshPanel();
}

void UWidgetPanelBase::ShowPanel_Implementation()
{
	SetActive(true);
}

void UWidgetPanelBase::HidePanel_Implementation()
{
	SetActive(false);
}

void UWidgetPanelBase::TogglePanel()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		ShowPanel();
	}
	else
	{
		HidePanel();
	}
}

void UWidgetPanelBase::RefreshPanel()
{
	K2_RefreshPanel();
}

bool UWidgetPanelBase::IsActive()
{
	return IsInViewport() && GetVisibility() != ESlateVisibility::Hidden;
}

AActor* UWidgetPanelBase::GetOwnerActor() const
{
	return nullptr;
}
