// // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
//
// #include "DWGameMode.h"
// #include "Character/Player/DWPlayerCharacter.h"
// #include "Character/Player/DWPlayerCharacterController.h"
// #include "DWGameState.h"
// #include "DWGameInstance.h"
// #include "Kismet/GameplayStatics.h"
// #include "World/WorldManager.h"
// #include "Kismet/KismetSystemLibrary.h"
//
// ADWGameMode::ADWGameMode()
// {
// 	// set default pawn class to our Blueprinted character
// 	
// }
//
// void ADWGameMode::BeginPlay()
// {
// 	Super::BeginPlay();
//
// }
//
// void ADWGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
// {
// 	Super::InitGame(MapName, Options, ErrorMessage);
//
// 	if(UDWGameInstance* DWGameInstance = UDWHelper::GetGameInstance(this))
// 	{
// 		DWGameInstance->LoadGameData();
// 	}
// }
//
// void ADWGameMode::StartPlay()
// {
// 	Super::StartPlay();
//
// 	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
// 	{
// 		DWGameState->SetCurrentState(EGameState::MainMenu);
// 	}
// }
//
// void ADWGameMode::StartGame()
// {
// 	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
// 	{
// 		if(AWorldManager* WorldManager = AWorldManager::Get())
// 		{
// 			WorldManager->LoadWorld(GameInstance->GetCurrentWorldName());
// 		}
// 		if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
// 		{
// 			PlayerController->LoadPlayer(GameInstance->GetCurrentPlayerName());
// 		}
// 	}
// }
//
// void ADWGameMode::PauseGame()
// {
// 	UGameplayStatics::SetGamePaused(this, true);
// 	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
// 	{
// 		DWGameState->SetCurrentState(EGameState::Pausing);
// 	}
// }
//
// void ADWGameMode::UnPauseGame()
// {
// 	UGameplayStatics::SetGamePaused(this, false);
// 	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
// 	{
// 		DWGameState->SetCurrentState(EGameState::Playing);
// 	}
// }
//
// void ADWGameMode::BackMainMenu()
// {
// 	if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
// 	{
// 		PlayerController->UnLoadPlayer();
// 	}
// 	if(AWorldManager* WorldManager = AWorldManager::Get())
// 	{
// 		WorldManager->UnloadWorld();
// 	}
// 	if(ADWGameState* DWGameState = UDWHelper::GetGameState(this))
// 	{
// 		if (DWGameState->GetCurrentState() == EGameState::Pausing)
// 		{
// 			UGameplayStatics::SetGamePaused(this, false);
// 		}
// 		DWGameState->SetCurrentState(EGameState::MainMenu);
// 	}
// }
//
// void ADWGameMode::QuitGame()
// {
// 	if(ADWPlayerCharacterController* PlayerController = UDWHelper::GetPlayerController(this))
// 	{
// 		PlayerController->UnLoadPlayer();
// 	}
// 	if(AWorldManager* WorldManager = AWorldManager::Get())
// 	{
// 		WorldManager->UnloadWorld();
// 	}
// 	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
// }
