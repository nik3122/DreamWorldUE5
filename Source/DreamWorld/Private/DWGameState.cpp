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
	ADWGameMode* GameMode = UDWHelper::GetGameMode();
	if (GameMode && CurrentState != InGameState)
	{
		CurrentState = InGameState;
		switch (InGameState)
		{
			case EGameState::MainMenu:
			{
				GameMode->ActivePermanentPanels(false);
				GameMode->ShowWidgetPanel(FName("MainMenu"));
				break;
			}
			case EGameState::ChoosingRole:
			{
				GameMode->ShowWidgetPanel(FName("RoleChoosingPanel"));
				break;
			}
			case EGameState::ChoosingWorld:
			{
				GameMode->ShowWidgetPanel(FName("WorldChoosingPanel"));
				break;
			}
			case EGameState::Loading:
			{
				GameMode->ShowWidgetPanel(FName("LoadingPanel"));
				break;
			}
			case EGameState::Playing:
			{
				GameMode->ActiveTemporaryPanel(false);
				GameMode->ShowWidgetPanel(FName("PrimaryPanel"));
				GameMode->ShowWidgetPanel(FName("InventoryBar"));
				break;
			}
			case EGameState::Pausing:
			{
				GameMode->ShowWidgetPanel(FName("PausingMenu"));
				break;
			}
		}
	}
}
