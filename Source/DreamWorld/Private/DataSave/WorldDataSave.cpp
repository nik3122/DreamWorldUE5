// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/WorldDataSave.h"

#include "Character/Player/DWPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldManager.h"

UWorldDataSave::UWorldDataSave()
{
	// set default pawn class to our Blueprinted character
	
	WorldData = FWorldData();
	ChunkDatas = TMap<FVector, FChunkData>();
	PlayerRecordDatas = TMap<FString, FPlayerRecordData>();
}

void UWorldDataSave::RefreshWorldData()
{
	if(AWorldManager* WorldManager = AWorldManager::GetCurrent())
	{
		WorldData = WorldManager->GetWorldData();
		WorldData.bSaved = true;
	}
}

bool UWorldDataSave::IsExistPlayerRecord(const FString& InPlayerName)
{
	return PlayerRecordDatas.Contains(InPlayerName);
}

void UWorldDataSave::SavePlayerRecord(FPlayerRecordData InPlayerRecordData)
{
	if (!PlayerRecordDatas.Contains(InPlayerRecordData.Name))
		PlayerRecordDatas.Add(InPlayerRecordData.Name, InPlayerRecordData);
	else
		PlayerRecordDatas[InPlayerRecordData.Name] = InPlayerRecordData;
}

FPlayerRecordData UWorldDataSave::LoadPlayerRecord(const FString& InPlayerName)
{
	if (IsExistPlayerRecord(InPlayerName))
	{
		return PlayerRecordDatas[InPlayerName];
	}
	return FPlayerRecordData();
}

void UWorldDataSave::RemovePlayerRecord(const FString& InPlayerName)
{
	if (IsExistPlayerRecord(InPlayerName))
	{
		PlayerRecordDatas.Remove(InPlayerName);
	}
}

bool UWorldDataSave::IsExistChunkData(FIndex InChunkIndex)
{
	return ChunkDatas.Contains(InChunkIndex.ToVector());
}

void UWorldDataSave::SaveChunkData(FIndex InChunkIndex, FChunkData InChunkData)
{
	if (!ChunkDatas.Contains(InChunkIndex.ToVector()))
		ChunkDatas.Add(InChunkIndex.ToVector(), InChunkData);
	else
		ChunkDatas[InChunkIndex.ToVector()] = InChunkData;
}

FChunkData UWorldDataSave::LoadChunkData(FIndex InChunkIndex)
{
	if (ChunkDatas.Contains(InChunkIndex.ToVector()))
		return ChunkDatas[InChunkIndex.ToVector()];
	return FChunkData();
}
