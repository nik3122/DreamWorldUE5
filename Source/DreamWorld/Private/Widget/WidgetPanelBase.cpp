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

bool UWidgetPanelBase::IsActive()
{
	return IsInViewport() && GetVisibility() != ESlateVisibility::Hidden;
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
		OnShowPanel();
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		if(WidgetType == EWidgetPanelType::Temporary)
		{
			if(IsInViewport()) RemoveFromViewport();
			UDWHelper::GetGameMode()->SetTemporaryPanel(nullptr);
		}
		OnHidePanel();
	}
	UDWHelper::GetGameMode()->UpdateInputMode();
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

void UWidgetPanelBase::OnShowPanel()
{
	
}

void UWidgetPanelBase::OnHidePanel()
{

}
