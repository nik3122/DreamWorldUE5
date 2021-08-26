// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWGameMode.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "Engine/World.h"
#include "DWGameState.h"
#include "DWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widget/WidgetPanelBase.h"
#include "UserWidget.h"

ADWGameMode::ADWGameMode()
{
	// set default pawn class to our Blueprinted character
	InputMode = EInputMode::None;
	TemporaryPanel = nullptr;
	PermanentPanels = TArray<UWidgetPanelBase*>();
	WidgetPanels = TMap<FName, UWidgetPanelBase*>();
}

void ADWGameMode::BeginPlay()
{
	Super::BeginPlay();

	GenerateWidgets();
	UDWHelper::GetGameState()->SetCurrentState(EGameState::MainMenu);
	UDWHelper::GetGameInstance()->LoadGameData();
}

void ADWGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void ADWGameMode::StartGame()
{
	UDWHelper::GetWorldManager()->LoadWorld();
}

void ADWGameMode::PauseGame()
{
	UGameplayStatics::SetGamePaused(this, true);
	UDWHelper::GetGameState()->SetCurrentState(EGameState::Pausing);
}

void ADWGameMode::ContinueGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	UDWHelper::GetGameState()->SetCurrentState(EGameState::Playing);
}

void ADWGameMode::BackMainMenu()
{
	if (!UDWHelper::GetWorldManager()->GetWorldName().IsEmpty())
	{
		UDWHelper::GetWorldManager()->UnloadWorld();
	}
	if (UDWHelper::GetGameState()->GetCurrentState() == EGameState::Pausing)
	{
		UGameplayStatics::SetGamePaused(this, false);
	}
	UDWHelper::GetGameState()->SetCurrentState(EGameState::MainMenu);
}

void ADWGameMode::ExitGame()
{
	if (!UDWHelper::GetWorldManager()->GetWorldName().IsEmpty())
	{
		//UDWHelper::GetWorldManager()->UnloadWorld();
	}
	UKismetSystemLibrary::QuitGame(this, UDWHelper::GetPlayerController(), EQuitPreference::Quit, true);
}

bool ADWGameMode::IsExistWidgetPanel(FName InWidgetName)
{
	return WidgetPanels.Contains(InWidgetName);
}

UWidgetPanelBase* ADWGameMode::GetWidgetPanel(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		return WidgetPanels[InWidgetName];
	}
	return nullptr;
}

UWidgetPanelBase* ADWGameMode::CreateWidgetPanel(TSubclassOf<UWidgetPanelBase> InWidgetClass, bool bShowPanel /*= false*/)
{
	FName WidgetName = FName();
	if (InWidgetClass != nullptr)
	{
		WidgetName = InWidgetClass.GetDefaultObject()->GetWidgetName();
	}
	if (!IsExistWidgetPanel(WidgetName) && InWidgetClass != nullptr)
	{
		UWidgetPanelBase* WidgetPanel = CreateWidget<UWidgetPanelBase>(GetWorld(), InWidgetClass, WidgetName);
		if (WidgetPanel != nullptr)
		{
			if (WidgetPanel->GetWidgetType() == EWidgetPanelType::Permanent)
			{
				PermanentPanels.Add(WidgetPanel);
			}
			WidgetPanel->SetActive(bShowPanel);
			WidgetPanels.Add(WidgetName, WidgetPanel);
		}
		return WidgetPanel;
	}
	
	return nullptr;
}

bool ADWGameMode::RemoveWidgetPanel(UWidgetPanelBase* InWidgetPanel)
{
	if (InWidgetPanel != nullptr)
	{
		return RemoveWidgetPanel(InWidgetPanel->GetWidgetName());
	}
	return false;
}

bool ADWGameMode::RemoveWidgetPanel(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		UWidgetPanelBase* WidgetPanel = GetWidgetPanel(InWidgetName);
		if (WidgetPanel != nullptr)
		{
			switch (WidgetPanel->GetWidgetType())
			{
				case EWidgetPanelType::Temporary:
				{
					TemporaryPanel = nullptr;
					break;
				}
				case EWidgetPanelType::Permanent:
				{
					PermanentPanels.Remove(WidgetPanel);
					break;
				}
			}
			WidgetPanel->RemoveFromViewport();
			WidgetPanel->ConditionalBeginDestroy();
			WidgetPanels.Remove(InWidgetName);
		}
		return true;
	}
	return false;
}

bool ADWGameMode::ToggleWidgetPanel(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanel(InWidgetName)->TogglePanel();
		return true;
	}
	return false;
}

bool ADWGameMode::ShowWidgetPanel(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanel(InWidgetName)->ShowPanel();
		return true;
	}
	return false;
}

bool ADWGameMode::HideWidgetPanel(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanel(InWidgetName)->HidePanel();
		return true;
	}
	return false;
}

void ADWGameMode::ActiveTemporaryPanel(bool bActive)
{
	if (TemporaryPanel)
	{
		TemporaryPanel->SetActive(bActive);
	}
}

void ADWGameMode::ActivePermanentPanels(bool bActive)
{
	for (UWidgetPanelBase* WidgetPanel : PermanentPanels)
	{
		WidgetPanel->SetActive(bActive);
	}
}

void ADWGameMode::ActiveWidgetPanels(bool bActive)
{
	for (auto iter =WidgetPanels.CreateConstIterator(); iter; ++iter)
	{
		iter->Value->SetActive(bActive);
	}
}

void ADWGameMode::UpdateInputMode()
{
	EInputMode inputMode = EInputMode::GameOnly;
	for (auto iter = WidgetPanels.CreateConstIterator(); iter; ++iter)
	{
		UWidgetPanelBase* widgetPanel = iter->Value;
		if (widgetPanel && widgetPanel->IsActive() && (int32)widgetPanel->GetInputMode() < (int32)inputMode)
		{
			inputMode = widgetPanel->GetInputMode();
		}
	}
	SetInputMode(inputMode);
}

void ADWGameMode::SetInputMode(EInputMode InInputMode)
{
	ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController();
	if(PlayerController && PlayerController->IsValidLowLevel() && InputMode != InInputMode)
	{
		InputMode = InInputMode;
		switch (InInputMode)
		{
			case EInputMode::None:
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = false;
				break;
			}
			case EInputMode::GameAndUI:
			{
				PlayerController->SetInputMode(FInputModeGameAndUI());
				PlayerController->bShowMouseCursor = true;
				break;
			}
			case EInputMode::GameOnly:
			{
				PlayerController->SetInputMode(FInputModeGameOnly());
				PlayerController->bShowMouseCursor = false;
				break;
			}
			case EInputMode::UIOnly:
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = true;
				break;
			}
		}
	}
}

void ADWGameMode::SetTemporaryPanel(UWidgetPanelBase* InTemporaryPanel)
{
	if (TemporaryPanel && InTemporaryPanel && TemporaryPanel != InTemporaryPanel)
	{
		TemporaryPanel->SetActive(false);
	}
	TemporaryPanel = InTemporaryPanel;
}
