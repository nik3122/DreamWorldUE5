// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWGameState.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "DWGameInstance.h"
#include "Public/DWGameMode.h"

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
	if(ADWGameMode* GameMode = UDWHelper::GetGameMode(this))
	{
		if (CurrentState != InGameState)
		{
			CurrentState = InGameState;
			switch (InGameState)
			{
				case EGameState::MainMenu:
				{
					GameMode->ActivePermanentPanels(false);
					GameMode->ShowWidgetPanelByName(FName("MainMenu"));
					break;
				}
				case EGameState::ChoosingRole:
				{
					GameMode->ShowWidgetPanelByName(FName("RoleChoosingPanel"));
					break;
				}
				case EGameState::ChoosingWorld:
				{
					GameMode->ShowWidgetPanelByName(FName("WorldChoosingPanel"));
					break;
				}
				case EGameState::Loading:
				{
					GameMode->ShowWidgetPanelByName(FName("LoadingPanel"));
					break;
				}
				case EGameState::Playing:
				{
					GameMode->ActiveTemporaryPanel(false);
					GameMode->ShowWidgetPanelByName(FName("PrimaryPanel"));
					GameMode->ShowWidgetPanelByName(FName("InventoryBar"));
					break;
				}
				case EGameState::Pausing:
				{
					GameMode->ShowWidgetPanelByName(FName("PausingMenu"));
					break;
				}
				default: break;
			}
		}
	}
}
