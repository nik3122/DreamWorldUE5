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
	WorldDataSaves = TMap<FString, UWorldDataSave*>();
	PlayerDataSaves = TMap<FString, UPlayerDataSave*>();

	UserIndex = 0;
}

void UDWGameInstance::Init()
{
	Super::Init();

	LoadGameData();
	UDWHelper::LoadItemDatas();
}

void UDWGameInstance::SaveGameData()
{
	if(GameDataSave)
	{
		GameDataSave->SaveGameData();
	}
}

UGameDataSave* UDWGameInstance::LoadGameData()
{
	if (GameDataSave && GameDataSave->IsValidLowLevel()) return GameDataSave;

	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameData"), UserIndex))
	{
		GameDataSave = Cast<UGameDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("GameData"), UserIndex));
		for(auto Iter : GameDataSave->GetWorldNames())
		{
			LoadWorldData(Iter);
		}
		for(auto Iter : GameDataSave->GetPlayerNames())
		{
			LoadPlayerData(Iter);
		}
	}
	else
	{
		GameDataSave = Cast<UGameDataSave>(UGameplayStatics::CreateSaveGameObject(UGameDataSave::StaticClass()));
		GameDataSave->SaveGameData(UserIndex);
	}
	return GameDataSave;
}

bool UDWGameInstance::IsExistWorldData(const FString& InWorldName)
{
	return WorldDataSaves.Contains(InWorldName);
}

void UDWGameInstance::SaveWorldData(const FString& InWorldName)
{
	if(UWorldDataSave* WorldDataSave = LoadWorldData(InWorldName))
	{
		WorldDataSave->SaveWorldData(UserIndex);
	}
}

UWorldDataSave* UDWGameInstance::LoadWorldData(const FString& InWorldName)
{
	if(IsExistWorldData(InWorldName)) return WorldDataSaves[InWorldName];
	
	if (UGameplayStatics::DoesSaveGameExist(TEXT("World_") + InWorldName, UserIndex))
	{
		if(UWorldDataSave* WorldDataSave = Cast<UWorldDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("World_") + InWorldName, UserIndex)))
		{
			WorldDataSaves.Add(InWorldName, WorldDataSave);
			UDWHelper::Debug(FString::Printf(TEXT("Loading world : %s, %dchunks."), *InWorldName, WorldDataSave->GetChunkDatas().Num()), EDebugType::Console);
			return WorldDataSave;
		}
	}
	return nullptr;
}

void UDWGameInstance::UnloadWorldData(const FString& InWorldName, bool bSaveData /*= true*/)
{
	if (!IsExistWorldData(InWorldName)) return;

	if(UWorldDataSave* WorldDataSave = LoadWorldData(InWorldName))
	{
		if(bSaveData)
		{
			WorldDataSave->SaveWorldData(UserIndex);
		}
		WorldDataSave->ConditionalBeginDestroy();
		WorldDataSaves.Remove(InWorldName);
	}
}

void UDWGameInstance::CreateWorldData(FWorldData InWorldData, bool bSaveData)
{
	if (IsExistWorldData(InWorldData.Name)) return;
	
	if(UWorldDataSave* WorldDataSave = Cast<UWorldDataSave>(UGameplayStatics::CreateSaveGameObject(UWorldDataSave::StaticClass())))
	{
		WorldDataSave->SetWorldData(InWorldData);
		WorldDataSaves.Add(InWorldData.Name, WorldDataSave);
		UDWHelper::Debug(FString::Printf(TEXT("Saving world : %s, %dchunks."), *WorldDataSave->GetWorldData().Name, WorldDataSave->GetChunkDatas().Num()), EDebugType::Console);
		if(bSaveData)
		{
			WorldDataSave->SaveWorldData(UserIndex);
		}
	}
}

void UDWGameInstance::RemoveWorldData(const FString& InWorldName)
{
	if (!IsExistWorldData(InWorldName)) return;

	if(UWorldDataSave* WorldDataSave = LoadWorldData(InWorldName))
	{
		for(auto Iter : GetPlayerDataSaves())
		{
			if(Iter->IsExistWorldRecord(InWorldName))
			{
				Iter->RemoveWorldRecord(InWorldName);
				Iter->SavePlayerData(UserIndex);
			}
		}
		WorldDataSave->RemoveWorldData(UserIndex);
		WorldDataSave->ConditionalBeginDestroy();
		WorldDataSaves.Remove(InWorldName);
	}
}

TArray<FString> UDWGameInstance::GetWorldNames() const
{
	TArray<FString> worldNames;
	WorldDataSaves.GenerateKeyArray(worldNames);
	return worldNames;
}

TArray<FWorldData> UDWGameInstance::GetWorldDatas() const
{
	TArray<FWorldData> worldDatas;
	for (auto iter : GetWorldDatas())
	{
		worldDatas.Add(iter);
	}
	return worldDatas;
}

TArray<UWorldDataSave*> UDWGameInstance::GetWorldDataSaves() const
{
	auto dataSaves = TArray<UWorldDataSave*>();
	for (auto iter : WorldDataSaves)
	{
		dataSaves.Add(iter.Value);
	}
	return dataSaves;
}

bool UDWGameInstance::IsExistPlayerData(const FString& InPlayerName)
{
	return PlayerDataSaves.Contains(InPlayerName);
}

void UDWGameInstance::SavePlayerData(const FString& InPlayerName)
{
	if(UPlayerDataSave* PlayerDataSave = LoadPlayerData(InPlayerName))
	{
		PlayerDataSave->SavePlayerData(UserIndex);
	}		
}

UPlayerDataSave* UDWGameInstance::LoadPlayerData(const FString& InPlayerName)
{
	if (IsExistPlayerData(InPlayerName)) return PlayerDataSaves[InPlayerName];

	if (UGameplayStatics::DoesSaveGameExist(TEXT("Player_") + InPlayerName, UserIndex))
	{
		if(UPlayerDataSave* PlayerDataSave = Cast<UPlayerDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Player_") + InPlayerName, UserIndex)))
		{
			PlayerDataSaves.Add(InPlayerName, PlayerDataSave);
			return PlayerDataSave;
		}
	}
	return nullptr;
}

void UDWGameInstance::UnloadPlayerData(const FString& InPlayerName, bool bSaveData /*= true*/)
{
	if (!IsExistPlayerData(InPlayerName)) return;

	if(UPlayerDataSave* PlayerDataSave = LoadPlayerData(InPlayerName))
	{
		if(bSaveData)
		{
			PlayerDataSave->SavePlayerData(UserIndex);
		}
		PlayerDataSave->ConditionalBeginDestroy();
		PlayerDataSaves.Remove(InPlayerName);
	}
}

void UDWGameInstance::CreatePlayerData(FCharacterData InPlayerData, bool bSaveData)
{
	if (IsExistPlayerData(InPlayerData.Name)) return;
	
	if(UPlayerDataSave* PlayerDataSave = Cast<UPlayerDataSave>(UGameplayStatics::CreateSaveGameObject(UPlayerDataSave::StaticClass())))
	{
		PlayerDataSave->SetPlayerData(InPlayerData);
		PlayerDataSaves.Add(InPlayerData.Name, PlayerDataSave);
		if(bSaveData)
		{
			PlayerDataSave->SavePlayerData(UserIndex);
		}
	}
}

void UDWGameInstance::RemovePlayerData(const FString& InPlayerName)
{
	if (!IsExistPlayerData(InPlayerName)) return;

	if(UPlayerDataSave* PlayerDataSave = LoadPlayerData(InPlayerName))
	{
		PlayerDataSave->RemovePlayerData(UserIndex);
		PlayerDataSave->ConditionalBeginDestroy();
		PlayerDataSaves.Remove(InPlayerName);
	}
}

TArray<FString> UDWGameInstance::GetPlayerNames() const
{
	TArray<FString> playerNames;
	PlayerDataSaves.GenerateKeyArray(playerNames);
	return playerNames;
}

TArray<FCharacterData> UDWGameInstance::GetPlayerDatas() const
{
	TArray<FCharacterData> playerDatas;
	for (auto iter : GetPlayerDatas())
	{
		playerDatas.Add(iter);
	}
	return playerDatas;
}

TArray<UPlayerDataSave*> UDWGameInstance::GetPlayerDataSaves() const
{
	auto dataSaves = TArray<UPlayerDataSave*>();
	for (auto iter : PlayerDataSaves)
	{
		dataSaves.Add(iter.Value);
	}
	return dataSaves;
}
