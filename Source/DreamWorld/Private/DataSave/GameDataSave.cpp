// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/GameDataSave.h"
#include "DataSave/WorldDataSave.h"
#include "DataSave/PlayerDataSave.h"
#include "Kismet/GameplayStatics.h"

UGameDataSave::UGameDataSave()
{
	// set default pawn class to our Blueprinted character

	UserIndex = 0;
	WorldDatas = TMap<FString, FWorldData>();
	PlayerDatas = TMap<FString, FCharacterData>();
}

bool UGameDataSave::IsExistWorldData(const FString& InWorldName)
{
	return WorldDatas.Contains(InWorldName);
}

bool UGameDataSave::IsExistPlayerData(const FString& InPlayerName)
{
	return PlayerDatas.Contains(InPlayerName);
}

void UGameDataSave::SaveGameData(const int32 InUserIndex /*= -1*/)
{
	if(InUserIndex != -1) UserIndex = InUserIndex;
	UGameplayStatics::SaveGameToSlot(this, TEXT("GameData"), UserIndex);
}

void UGameDataSave::CreateWorldData(FWorldData InSaveData)
{
	if (!IsExistWorldData(InSaveData.Name))
	{
		WorldDatas.Add(InSaveData.Name, InSaveData);
		auto worldDataSave = Cast<UWorldDataSave>(UGameplayStatics::CreateSaveGameObject(UWorldDataSave::StaticClass()));
		worldDataSave->SetWorldData(InSaveData);
		SaveWorldData(worldDataSave);
	}
	SaveGameData();
}

void UGameDataSave::CreatePlayerData(FCharacterData InSaveData)
{
	if (!IsExistPlayerData(InSaveData.Name))
	{
		PlayerDatas.Add(InSaveData.Name, InSaveData);
		auto playerDataSave = Cast<UPlayerDataSave>(UGameplayStatics::CreateSaveGameObject(UPlayerDataSave::StaticClass()));
		playerDataSave->SetPlayerData(InSaveData);
		SavePlayerData(playerDataSave);
	}
	SaveGameData();
}

void UGameDataSave::SaveWorldData(UWorldDataSave* InWorldDataSave)
{
	if (IsExistWorldData(InWorldDataSave->GetWorldData().Name))
	{
		UDWHelper::Debug(FString::Printf(TEXT("Saving world : %s, %dchunks."), *InWorldDataSave->GetWorldData().Name, InWorldDataSave->GetChunkDatas().Num()), EDebugType::Console);
		UGameplayStatics::SaveGameToSlot(InWorldDataSave, TEXT("World_") + InWorldDataSave->GetWorldData().Name, UserIndex);
	}
}

void UGameDataSave::SavePlayerData(UPlayerDataSave* InPlayerDataSave)
{
	if (IsExistPlayerData(InPlayerDataSave->GetPlayerData().Name))
	{
		UGameplayStatics::SaveGameToSlot(InPlayerDataSave, TEXT("Player_") + InPlayerDataSave->GetPlayerData().Name, UserIndex);
	}
}

void UGameDataSave::RemoveWorldData(const FString& InWorldName)
{
	if (IsExistWorldData(InWorldName))
	{
		auto dataSaves = GetPlayerDataSaves();
		for (int32 i = 0; i < dataSaves.Num(); i++)
		{
			dataSaves[i]->RemoveWorldRecord(InWorldName);
			SavePlayerData(dataSaves[i]);
		}
		if (UGameplayStatics::DoesSaveGameExist(TEXT("World_") + InWorldName, UserIndex))
		{
			UGameplayStatics::DeleteGameInSlot(TEXT("World_") + InWorldName, UserIndex);
		}
		WorldDatas.Remove(InWorldName);
	}
	SaveGameData();
}

void UGameDataSave::RemovePlayerData(const FString& InPlayerName)
{
	if (IsExistPlayerData(InPlayerName))
	{
		if (UGameplayStatics::DoesSaveGameExist(TEXT("Player_") + InPlayerName, UserIndex))
		{
			UGameplayStatics::DeleteGameInSlot(TEXT("Player_") + InPlayerName, UserIndex);
		}
		PlayerDatas.Remove(InPlayerName);
	}
	SaveGameData();
}

UWorldDataSave* UGameDataSave::LoadWorldData(const FString& InWorldName)
{
	if (IsExistWorldData(InWorldName) && UGameplayStatics::DoesSaveGameExist(TEXT("World_") + InWorldName, UserIndex))
	{
		return Cast<UWorldDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("World_") + InWorldName, UserIndex));
	}
	return nullptr;
}

UPlayerDataSave* UGameDataSave::LoadPlayerData(const FString& InPlayerName)
{
	if (IsExistPlayerData(InPlayerName) && UGameplayStatics::DoesSaveGameExist(TEXT("Player_") + InPlayerName, UserIndex))
	{
		return Cast<UPlayerDataSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Player_") + InPlayerName, UserIndex));
	}
	return nullptr;
}

TArray<FString> UGameDataSave::GetWorldNames()
{
	TArray<FString> worldNames;
	WorldDatas.GenerateKeyArray(worldNames);
	return worldNames;
}

TArray<FString> UGameDataSave::GetPlayerNames()
{
	TArray<FString> playerNames;
	PlayerDatas.GenerateKeyArray(playerNames);
	return playerNames;
}

TArray<FWorldData> UGameDataSave::GetWorldDatas()
{
	TArray<FWorldData> worldDatas;
	WorldDatas.GenerateValueArray(worldDatas);
	return worldDatas;
}

TArray<FCharacterData> UGameDataSave::GetPlayerDatas()
{
	TArray<FCharacterData> playerDatas;
	if(PlayerDatas.Num() > 0)
	{
		PlayerDatas.GenerateValueArray(playerDatas);
	}
	return playerDatas;
}

TArray<UWorldDataSave*> UGameDataSave::GetWorldDataSaves()
{
	auto dataSaves = TArray<UWorldDataSave*>();
	for (auto iter = WorldDatas.CreateConstIterator(); iter; ++iter)
	{
		dataSaves.Add(LoadWorldData(iter->Key));
	}
	return dataSaves;
}

TArray<UPlayerDataSave*> UGameDataSave::GetPlayerDataSaves()
{
	auto dataSaves = TArray<UPlayerDataSave*>();
	for (auto iter = PlayerDatas.CreateConstIterator(); iter; ++iter)
	{
		dataSaves.Add(LoadPlayerData(iter->Key));
	}
	return dataSaves;
}
