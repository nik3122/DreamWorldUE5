// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetPrimaryPanel.h"

#include "DWPlayerCharacter.h"
#include "WidgetModule.h"
#include "WidgetModuleBPLibrary.h"

UWidgetPrimaryPanel::UWidgetPrimaryPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetName = FName("PrimaryPanel");
	WidgetType = EWidgetType::Permanent;
	InputMode = EInputMode::GameOnly;
}

void UWidgetPrimaryPanel::OnCreate_Implementation()
{
	if(AWidgetModule* WidgetModule = AMainModule::GetModuleByClass<AWidgetModule>())
	{
		WidgetModule->OnChangeInputMode.AddDynamic(this, &UWidgetPrimaryPanel::OnChangeInputMode);
	}
}

void UWidgetPrimaryPanel::RefreshOptions()
{
	if(ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(GetOwnerActor()))
	{
		if(OwnerCharacter->GetInteractingTarget())
		{
			ShowOptions(OwnerCharacter->GetInteractingTarget()->GetInteractOptions(OwnerCharacter));
		}
		else
		{
			HideOptions();
		}
	}
}

void UWidgetPrimaryPanel::OnChangeInputMode(EInputMode InInputMode)
{
	if(ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerActor))
	{
		if(!OwnerCharacter->IsDead() && InInputMode == EInputMode::GameOnly)
		{
			SetCrosshairVisible(true);
		}
		else
		{
			SetCrosshairVisible(true);
		}
	}
	else
	{
		SetCrosshairVisible(true);
	}
}
