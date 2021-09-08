// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/WorldDataSave.h"

#include "WorldTimerComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "World/Chunk.h"
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
	if(AWorldManager* WorldManager = AWorldManager::Get())
	{
		WorldData = WorldManager->GetData();

		if(ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this))
		{
			FPlayerRecordData RecordData;
			RecordData.Name = PlayerCharacter->GetName();
			RecordData.Location = PlayerCharacter->GetActorLocation();
			RecordData.Rotation = PlayerCharacter->GetActorRotation();
			
			if(UWorldTimerComponent* WorldTimer = WorldManager->GetWorldTimer())
			{
				RecordData.TimeSeconds = WorldTimer->GetTimeSeconds();
			}

			if (!PlayerRecordDatas.Contains(RecordData.Name))
				PlayerRecordDatas.Add(RecordData.Name, RecordData);
			else
				PlayerRecordDatas[RecordData.Name] = RecordData;
		}
	}
}

bool UWorldDataSave::IsExistPlayerRecord(const FString& InPlayerName)
{
	return PlayerRecordDatas.Contains(InPlayerName);
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
