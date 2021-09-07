// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWGameMode.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
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
	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
	{
		DWGameState->SetCurrentState(EGameState::MainMenu);
	}
	if(UDWGameInstance* DWGameInstance = UDWHelper::GetGameInstance(this))
	{
		DWGameInstance->LoadGameData();
	}
}

void ADWGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void ADWGameMode::StartGame()
{
	AWorldManager::Get()->LoadWorld();
}

void ADWGameMode::PauseGame()
{
	UGameplayStatics::SetGamePaused(this, true);
	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
	{
		DWGameState->SetCurrentState(EGameState::Pausing);
	}
}

void ADWGameMode::UnPauseGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
	{
		DWGameState->SetCurrentState(EGameState::Playing);
	}
}

void ADWGameMode::BackMainMenu()
{
	if (!AWorldManager::GetInfo().WorldName.IsEmpty())
	{
		AWorldManager::Get()->UnloadWorld();
	}
	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
	{
		if (DWGameState->GetCurrentState() == EGameState::Pausing)
		{
			UGameplayStatics::SetGamePaused(this, false);
		}
		DWGameState->SetCurrentState(EGameState::MainMenu);
	}
}

void ADWGameMode::QuitGame()
{
	if (!AWorldManager::GetInfo().WorldName.IsEmpty())
	{
		//AWorldManager::GetCurrent()->UnloadWorld();
	}
	if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
	{
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
	}
}

bool ADWGameMode::IsExistWidgetPanel(FName InWidgetName) const
{
	return WidgetPanels.Contains(InWidgetName);
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

UWidgetPanelBase* ADWGameMode::GetWidgetPanelByName(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		return WidgetPanels[InWidgetName];
	}
	return nullptr;
}

UWidgetPanelBase* ADWGameMode::K2_GetWidgetPanelByClass(TSubclassOf<UWidgetPanelBase> InWidgetPanelClass)
{
	if(UWidgetPanelBase* TmpObject = Cast<UWidgetPanelBase>(InWidgetPanelClass->GetDefaultObject()))
	{
		return GetWidgetPanelByName(TmpObject->GetWidgetName());
	}
	return nullptr;
}

bool ADWGameMode::RemoveWidgetPanelByName(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		UWidgetPanelBase* WidgetPanel = GetWidgetPanelByName(InWidgetName);
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

bool ADWGameMode::ToggleWidgetPanelByName(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanelByName(InWidgetName)->TogglePanel();
		return true;
	}
	return false;
}

bool ADWGameMode::ShowWidgetPanelByName(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanelByName(InWidgetName)->ShowPanel();
		return true;
	}
	return false;
}

bool ADWGameMode::HideWidgetPanelByName(FName InWidgetName)
{
	if (IsExistWidgetPanel(InWidgetName))
	{
		GetWidgetPanelByName(InWidgetName)->HidePanel();
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
	if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
	{
		if(InputMode != InInputMode)
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
}

void ADWGameMode::SetTemporaryPanel(UWidgetPanelBase* InTemporaryPanel)
{
	if (TemporaryPanel && InTemporaryPanel && TemporaryPanel != InTemporaryPanel)
	{
		TemporaryPanel->SetActive(false);
	}
	TemporaryPanel = InTemporaryPanel;
}

TArray<UWidgetPanelBase*> ADWGameMode::GetWidgetPanels() const
{
	TArray<UWidgetPanelBase*> TmpArr;
	WidgetPanels.GenerateValueArray(TmpArr);
	return TmpArr;
}
