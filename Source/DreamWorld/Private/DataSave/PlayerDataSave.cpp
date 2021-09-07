// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/PlayerDataSave.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldManager.h"
#include "World/Components/WorldTimerComponent.h"

UPlayerDataSave::UPlayerDataSave()
{
	// set default pawn class to our Blueprinted character
	
	PlayerData = FCharacterData();
	WorldRecordDatas = TMap<FString, FWorldRecordData>();
}

void UPlayerDataSave::SavePlayerData(const int32 InUserIndex)
{
	if(ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this))
	{
		PlayerData = PlayerCharacter->ToData();

		auto SaveData = FWorldRecordData();
		SaveData.WorldName = AWorldManager::GetInfo().WorldName;
		SaveData.PlayerLocation = PlayerCharacter->GetActorLocation();
		SaveData.PlayerRotation = PlayerCharacter->GetActorRotation();
		SaveData.TimeSeconds = AWorldManager::Get()->GetWorldTimer()->GetTimeSeconds();

		if (!WorldRecordDatas.Contains(AWorldManager::GetInfo().WorldName))
			WorldRecordDatas.Add(AWorldManager::GetInfo().WorldName, SaveData);
		else
			WorldRecordDatas[AWorldManager::GetInfo().WorldName] = SaveData;

		UGameplayStatics::SaveGameToSlot(this, TEXT("Player_") + GetPlayerData().Name, InUserIndex);
	}
}

void UPlayerDataSave::RemovePlayerData(const int32 InUserIndex)
{
	UGameplayStatics::DeleteGameInSlot(TEXT("Player_") + GetPlayerData().Name, InUserIndex);
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
