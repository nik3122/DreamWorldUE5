// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/PlayerDataSave.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "World/WorldManager.h"
#include "World/Components/WorldTimerComponent.h"

UPlayerDataSave::UPlayerDataSave()
{
	// set default pawn class to our Blueprinted character
	
	PlayerData = FCharacterData();
	WorldRecordDatas = TMap<FString, FWorldRecordData>();
}

void UPlayerDataSave::SavePlayerData(ADWPlayerCharacter* InPlayerCharacter)
{
	if(!InPlayerCharacter) return;
	
	PlayerData = InPlayerCharacter->ToData();

	auto SaveData = FWorldRecordData();
	SaveData.WorldName = AWorldManager::GetWorldInfo().WorldName;
	SaveData.PlayerLocation = InPlayerCharacter->GetActorLocation();
	SaveData.PlayerRotation = InPlayerCharacter->GetActorRotation();
	SaveData.TimeSeconds = AWorldManager::GetCurrent()->GetWorldTimer()->GetTimeSeconds();

	if (!WorldRecordDatas.Contains(AWorldManager::GetWorldInfo().WorldName))
		WorldRecordDatas.Add(AWorldManager::GetWorldInfo().WorldName, SaveData);
	else
		WorldRecordDatas[AWorldManager::GetWorldInfo().WorldName] = SaveData;
}

bool UPlayerDataSave::IsExistWorldRecord(const FString& InWorldName)
{
	return WorldRecordDatas.Contains(InWorldName);
}

FWorldRecordData UPlayerDataSave::LoadWorldRecord(const FString& InWorldName)
{
	if (IsExistWorldRecord(InWorldName))
	{
		return WorldRecordDatas[InWorldName];
	}
	return FWorldRecordData();
}

void UPlayerDataSave::RemoveWorldRecord(const FString& InWorldName)
{
	if (IsExistWorldRecord(InWorldName))
	{
		WorldRecordDatas.Remove(InWorldName);
	}
}
