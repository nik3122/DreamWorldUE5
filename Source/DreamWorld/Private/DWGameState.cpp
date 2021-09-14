// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWGameState.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "DWGameInstance.h"
#include "WidgetInventoryBar.h"
#include "WidgetLoadingPanel.h"
#include "WidgetMainMenu.h"
#include "WidgetModuleBPLibrary.h"
#include "WidgetPausingMenu.h"
#include "WidgetPrimaryPanel.h"
#include "WidgetRoleChoosingPanel.h"
#include "WidgetWorldChoosingPanel.h"

ADWGameState::ADWGameState()
{
	// set default pawn class to our Blueprinted character
	
	CurrentState = EGameState::None;
}

void ADWGameState::BeginPlay()
{
	Super::BeginPlay();

}

void ADWGameState::SetCurrentState(EGameState InGameState)
{
	if (CurrentState != InGameState)
	{
		CurrentState = InGameState;
		switch (InGameState)
		{
			case EGameState::MainMenu:
			{
				UWidgetModuleBPLibrary::CloseAllUserWidget(this, EWidgetType::Permanent);
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetMainMenu>(this);
				break;
			}
			case EGameState::ChoosingRole:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetRoleChoosingPanel>(this);
				break;
			}
			case EGameState::ChoosingWorld:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetWorldChoosingPanel>(this);
				break;
			}
			case EGameState::Loading:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetLoadingPanel>(this);
				break;
			}
			case EGameState::Playing:
			{
				UWidgetModuleBPLibrary::CloseAllUserWidget(this, EWidgetType::Temporary);
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetPrimaryPanel>(this);
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetInventoryBar>(this);
				break;
			}
			case EGameState::Pausing:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetPausingMenu>(this);
				break;
			}
			default: break;
		}
	}
}
