// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Gameplay/DWGameState.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "Gameplay/DWGameInstance.h"
#include "WidgetInventoryBar.h"
#include "WidgetInventoryPanel.h"
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
				UWidgetModuleBPLibrary::CloseAllUserWidget(EWidgetType::Permanent);
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetMainMenu>(false, UDWHelper::LoadWidgetMainMenuClass());
				break;
			}
			case EGameState::ChoosingRole:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetRoleChoosingPanel>(false, UDWHelper::LoadWidgetRoleChoosingPanelClass());
				break;
			}
			case EGameState::ChoosingWorld:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetWorldChoosingPanel>(false, UDWHelper::LoadWidgetWorldChoosingPanelClass());
				break;
			}
			case EGameState::Loading:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetLoadingPanel>(false, UDWHelper::LoadWidgetLoadingPanelClass());
				UWidgetModuleBPLibrary::CreateUserWidget<UWidgetPrimaryPanel>(UDWHelper::LoadWidgetPrimaryPanelClass());
				UWidgetModuleBPLibrary::CreateUserWidget<UWidgetInventoryBar>(UDWHelper::LoadWidgetInventoryBarClass());
				UWidgetModuleBPLibrary::CreateUserWidget<UWidgetInventoryPanel>(UDWHelper::LoadWidgetInventoryPanelClass());
				break;
			}
			case EGameState::Playing:
			{
				UWidgetModuleBPLibrary::CloseAllUserWidget(EWidgetType::Temporary);
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetPrimaryPanel>(false, UDWHelper::LoadWidgetPrimaryPanelClass());
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetInventoryBar>(false, UDWHelper::LoadWidgetInventoryBarClass());
				break;
			}
			case EGameState::Pausing:
			{
				UWidgetModuleBPLibrary::OpenUserWidget<UWidgetPausingMenu>(false, UDWHelper::LoadWidgetPausingMenuClass());
				break;
			}
			default: break;
		}
	}
}
