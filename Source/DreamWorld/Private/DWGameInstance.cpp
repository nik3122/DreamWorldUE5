// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWGameInstance.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "Engine/World.h"
#include "Widget/WidgetPanelBase.h"
#include "Widget/WidgetMainMenu.h"
#include "Widget/WidgetPausingMenu.h"
#include "Widget/WidgetLoadingPanel.h"
#include "Widget/WidgetSettingPanel.h"
#include "Widget/WidgetRoleChoosingPanel.h"
#include "Widget/WidgetRoleCreatingPanel.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Widget/WidgetWorldChoosingPanel.h"
#include "Widget/WidgetWorldCreatingPanel.h"
#include "Widget/WidgetPrimaryPanel.h"
#include "Widget/Inventory/WidgetInventoryBox.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "DWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "DataSave/GameDataSave.h"
#include "DataSave/WorldDataSave.h"
#include "DataSave/PlayerDataSave.h"
#include "World/WorldManager.h"
#include "DWGameState.h"

UDWGameInstance::UDWGameInstance()
{
	// set default pawn class to our Blueprinted character

	GameDataSave = nullptr;
	WorldDataSave = nullptr;
	PlayerDataSave = nullptr;
}

void UDWGameInstance::Init()
{
	Super::Init();

	UDWHelper::LoadItemDatas();
}

bool UDWGameInstance::IsExistWorldData(const FString& InWorldName)
{
	return GameDataSave->IsExistWorldData(InWorldName);
}

bool UDWGameInstance::IsExistPlayerData(const FString& InPlayerName)
{
	return GameDataSave->IsExistPlayerData(InPlayerName);
}

void UDWGameInstance::SaveGameData()
{
	GameDataSave->SaveGameData();
}

void UDWGameInstance::SaveWorldData()
{
	WorldDataSave->SaveWorldData(AWorldManager::GetCurrent());
	GameDataSave->SaveWorldData(WorldDataSave);
}

void UDWGameInstance::SavePlayerData()
{
	PlayerDataSave->SavePlayerData(UDWHelper::GetPlayerCharacter(this));
	GameDataSave->SavePlayerData(PlayerDataSave);
}

void UDWGameInstance::LoadGameData(const int32 InUserIndex)
{
	if (GameDataSave && GameDataSave->IsValidLowLevel()) return;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameData"), InUserIndex))
	{
		GameDataSave = Cast<UGameDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("GameData"), InUserIndex));
	}
	else
	{
		GameDataSave = Cast<UGameDataSave>(UGameplayStatics::CreateSaveGameObject(UGameDataSave::StaticClass()));
		GameDataSave->SaveGameData(InUserIndex);
	}
}

void UDWGameInstance::LoadWorldData(const FString& InWorldName)
{
	if (WorldDataSave) return;
	
	WorldDataSave = GameDataSave->LoadWorldData(InWorldName);

	UDWHelper::Debug(FString::Printf(TEXT("Loading world : %s, %dchunks."), *InWorldName, WorldDataSave->GetChunkDatas().Num()), EDebugType::Console);
}

void UDWGameInstance::LoadPlayerData(const FString& InPlayerName)
{
	if (PlayerDataSave) return;

	PlayerDataSave = GameDataSave->LoadPlayerData(InPlayerName);
}

void UDWGameInstance::UnloadWorldData(bool bData /*= true*/)
{
	if(bData) SaveWorldData();
	WorldDataSave->ConditionalBeginDestroy();
	WorldDataSave = nullptr;
}

void UDWGameInstance::UnloadPlayerData(bool bData /*= true*/)
{
 	if(bData) SavePlayerData();
	PlayerDataSave->ConditionalBeginDestroy();
	PlayerDataSave = nullptr;
}

void UDWGameInstance::CreateWorldData(FWorldData InWorldData, bool bAutoLoad /*= true*/)
{
	GameDataSave->CreateWorldData(InWorldData);
	if (bAutoLoad) LoadWorldData(InWorldData.Name);
}

void UDWGameInstance::CreatePlayerData(FCharacterData InPlayerData, bool bAutoLoad /*= true*/)
{
	GameDataSave->CreatePlayerData(InPlayerData);
	if (bAutoLoad) LoadPlayerData(InPlayerData.Name);
}

void UDWGameInstance::RemoveWorldData(const FString& InWorldName)
{
	GameDataSave->RemoveWorldData(InWorldName);
}

void UDWGameInstance::RemovePlayerData(const FString& InPlayerName)
{
	GameDataSave->RemovePlayerData(InPlayerName);
}

TArray<FString> UDWGameInstance::GetWorldNames()
{
	return GameDataSave->GetWorldNames();
}

TArray<FString> UDWGameInstance::GetPlayerNames()
{
	return GameDataSave->GetPlayerNames();
}

TArray<FWorldData> UDWGameInstance::GetWorldDatas()
{
	return GameDataSave->GetWorldDatas();
}

TArray<FCharacterData> UDWGameInstance::GetPlayerDatas()
{
	return GameDataSave->GetPlayerDatas();
}

TArray<UWorldDataSave*> UDWGameInstance::GetWorldDataSaves()
{
	return GameDataSave->GetWorldDataSaves();
}

TArray<UPlayerDataSave*> UDWGameInstance::GetPlayerDataSaves()
{
	return GameDataSave->GetPlayerDataSaves();
}
