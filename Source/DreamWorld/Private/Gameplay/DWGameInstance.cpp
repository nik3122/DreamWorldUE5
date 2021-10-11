// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Gameplay/DWGameInstance.h"

#include "DebugModuleTypes.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Gameplay/DWPlayerController.h"
#include "Engine/World.h"
#include "UserWidgetBase.h"
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
#include "Gameplay/DWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "DataSave/GameDataSave.h"
#include "DataSave/WorldDataSave.h"
#include "DataSave/PlayerDataSave.h"
#include "World/WorldManager.h"
#include "Gameplay/DWGameState.h"

UDWGameInstance::UDWGameInstance()
{
	// set default pawn class to our Blueprinted character

	GameDataSave = nullptr;
	WorldDataSaves = TMap<FString, UWorldDataSave*>();
	PlayerDataSaves = TMap<FString, UPlayerDataSave*>();

	UserIndex = 0;
}

void UDWGameInstance::Init()
{
	Super::Init();

	UDWHelper::LoadItemDatas();
}

void UDWGameInstance::Shutdown()
{
	Super::Shutdown();

}

void UDWGameInstance::Initialize()
{
	ResetGameData();
	LoadGameData();
}

void UDWGameInstance::ResetGameData()
{
	GameDataSave = nullptr;
	CurrentWorldName = TEXT("");
	CurrentPlayerName = TEXT("");
	WorldDataSaves.Empty();
	PlayerDataSaves.Empty();
}

void UDWGameInstance::SaveGameData(bool bRefresh)
{
	if(GameDataSave)
	{
		if(bRefresh) GameDataSave->RefreshGameData();
		UGameplayStatics::SaveGameToSlot(GameDataSave, TEXT("GameData"), UserIndex);
	}
}

UGameDataSave* UDWGameInstance::LoadGameData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameData"), UserIndex))
	{
		GameDataSave = Cast<UGameDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("GameData"), UserIndex));
	}
	else
	{
		CreateGameData(FGameSaveData());
	}

	return GameDataSave;
}

void UDWGameInstance::CreateGameData(FGameSaveData InGameData, bool bSaveData)
{
	GameDataSave = Cast<UGameDataSave>(UGameplayStatics::CreateSaveGameObject(UGameDataSave::StaticClass()));
	if(GameDataSave)
	{
		GameDataSave->SetGameData(InGameData);
		if(bSaveData) SaveGameData(false);
	}
}

void UDWGameInstance::DeleteGameData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameData"), UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(TEXT("GameData"), UserIndex);
	}
	GameDataSave = nullptr;
}

FGameSaveData UDWGameInstance::GetGameData() const
{
	if(GameDataSave)
	{
		return GameDataSave->GetGameData();
	}
	return FGameSaveData();
}

bool UDWGameInstance::IsExistWorldData(const FString& InWorldName)
{
	return GetWorldDatas().Contains(InWorldName);
}

void UDWGameInstance::SaveWorldData(const FString& InWorldName, bool bRefresh)
{
	if(UWorldDataSave* WorldDataSave = LoadWorldData(InWorldName))
	{
		if(bRefresh) WorldDataSave->RefreshWorldData();
		
		WHDebug(FString::Printf(TEXT("Saving world : %s, %dchunks."), *InWorldName, WorldDataSave->GetChunkDatas().Num()), FColor::Cyan);

		UGameplayStatics::SaveGameToSlot(WorldDataSave, TEXT("World_") + InWorldName, UserIndex);
	}
}

UWorldDataSave* UDWGameInstance::LoadWorldData(const FString& InWorldName)
{
	if(!InWorldName.IsEmpty()) CurrentWorldName = InWorldName;
	if(WorldDataSaves.Contains(InWorldName)) return WorldDataSaves[CurrentWorldName];
	
	if (UGameplayStatics::DoesSaveGameExist(TEXT("World_") + CurrentWorldName, UserIndex))
	{
		if(UWorldDataSave* WorldDataSave = Cast<UWorldDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("World_") + CurrentWorldName, UserIndex)))
		{
			WorldDataSaves.Add(CurrentWorldName, WorldDataSave);
			WHDebug(FString::Printf(TEXT("Loading world : %s, %dchunks."), *CurrentWorldName, WorldDataSave->GetChunkDatas().Num()), FColor::Cyan);
			return WorldDataSave;
		}
	}
	return nullptr;
}

void UDWGameInstance::UnloadWorldData(const FString& InWorldName, bool bSaveData /*= true*/)
{
	if(!WorldDataSaves.Contains(InWorldName)) return;
	
	if(bSaveData) SaveWorldData(InWorldName);
	WorldDataSaves[InWorldName]->ConditionalBeginDestroy();
	WorldDataSaves.Remove(InWorldName);
	CurrentWorldName = TEXT("");
}

void UDWGameInstance::CreateWorldData(FWorldSaveData InWorldData, bool bSaveData)
{
	if (IsExistWorldData(InWorldData.Name)) return;
	
	if(UWorldDataSave* WorldDataSave = Cast<UWorldDataSave>(UGameplayStatics::CreateSaveGameObject(UWorldDataSave::StaticClass())))
	{
		CurrentWorldName = InWorldData.Name;
		WorldDataSave->SetWorldData(InWorldData);
		WorldDataSaves.Add(InWorldData.Name, WorldDataSave);
		if(GameDataSave)
		{
			GameDataSave->GetWorldDatas().Add(InWorldData.Name, InWorldData);
		}
		if(bSaveData)
		{
			SaveWorldData(InWorldData.Name, false);
			SaveGameData(false);
		}
	}
}

void UDWGameInstance::RemoveWorldData(const FString& InWorldName)
{
	if (!IsExistWorldData(InWorldName)) return;

	if(GameDataSave)
	{
		UnloadWorldData(InWorldName, false);
		UGameplayStatics::DeleteGameInSlot(TEXT("World_") + InWorldName, UserIndex);
		GameDataSave->GetWorldDatas().Remove(InWorldName);
		SaveGameData(false);
	}
}

TMap<FString, FWorldBasicSaveData> UDWGameInstance::GetWorldDatas() const
{
	if(GameDataSave)
	{
		return GameDataSave->GetWorldDatas();
	}
	return TMap<FString, FWorldBasicSaveData>();
}

bool UDWGameInstance::IsExistPlayerData(const FString& InPlayerName)
{
	return GetPlayerDatas().Contains(InPlayerName);
}

void UDWGameInstance::SavePlayerData(const FString& InPlayerName, bool bRefresh)
{
	if(UPlayerDataSave* PlayerDataSave = LoadPlayerData(InPlayerName))
	{
		if(bRefresh) PlayerDataSave->RefreshPlayerData();
		UGameplayStatics::SaveGameToSlot(PlayerDataSave, TEXT("Player_") + InPlayerName, UserIndex);
	}
}

UPlayerDataSave* UDWGameInstance::LoadPlayerData(const FString& InPlayerName)
{
	if(!InPlayerName.IsEmpty()) CurrentPlayerName = InPlayerName;
	if (PlayerDataSaves.Contains(CurrentPlayerName)) return PlayerDataSaves[CurrentPlayerName];

	if (UGameplayStatics::DoesSaveGameExist(TEXT("Player_") + CurrentPlayerName, UserIndex))
	{
		if(UPlayerDataSave* PlayerDataSave = Cast<UPlayerDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Player_") + CurrentPlayerName, UserIndex)))
		{
			PlayerDataSaves.Add(CurrentPlayerName, PlayerDataSave);
			return PlayerDataSave;
		}
	}
	return nullptr;
}

void UDWGameInstance::UnloadPlayerData(const FString& InPlayerName, bool bSaveData /*= true*/)
{
	if (!PlayerDataSaves.Contains(CurrentPlayerName)) return;

	if(bSaveData) SavePlayerData(InPlayerName);
	PlayerDataSaves[CurrentPlayerName]->ConditionalBeginDestroy();
	PlayerDataSaves.Remove(InPlayerName);
	CurrentPlayerName = TEXT("");
}

void UDWGameInstance::CreatePlayerData(FCharacterSaveData InPlayerData, bool bSaveData)
{
	if (IsExistPlayerData(InPlayerData.Name)) return;
	
	if(UPlayerDataSave* PlayerDataSave = Cast<UPlayerDataSave>(UGameplayStatics::CreateSaveGameObject(UPlayerDataSave::StaticClass())))
	{
		CurrentPlayerName = InPlayerData.Name;
		PlayerDataSave->SetPlayerData(InPlayerData);
		PlayerDataSaves.Add(InPlayerData.Name, PlayerDataSave);
		if(GameDataSave)
		{
			GameDataSave->GetPlayerDatas().Add(InPlayerData.Name, InPlayerData);
		}
		if(bSaveData)
		{
			SavePlayerData(InPlayerData.Name, false);
			SaveGameData(false);
		}
	}
}

void UDWGameInstance::RemovePlayerData(const FString& InPlayerName)
{
	if (!IsExistPlayerData(InPlayerName)) return;

	if(GameDataSave)
	{
		UnloadPlayerData(InPlayerName, false);
		UGameplayStatics::DeleteGameInSlot(TEXT("Player_") + InPlayerName, UserIndex);
		GameDataSave->GetPlayerDatas().Remove(InPlayerName);
		SaveGameData(false);
	}
}

TMap<FString, FCharacterBasicSaveData> UDWGameInstance::GetPlayerDatas() const
{
	if(GameDataSave)
	{
		return GameDataSave->GetPlayerDatas();
	}
	return TMap<FString, FCharacterBasicSaveData>();
}
