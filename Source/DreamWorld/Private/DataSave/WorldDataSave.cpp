// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/WorldDataSave.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "World/WorldManager.h"

UWorldDataSave::UWorldDataSave()
{
	// set default pawn class to our Blueprinted character
	
	WorldData = FWorldData();
	ChunkDatas = TMap<FVector, FChunkData>();
}

bool UWorldDataSave::IsExistChunkData(FIndex InChunkIndex)
{
	return ChunkDatas.Contains(InChunkIndex.ToVector());
}

void UWorldDataSave::SaveWorldData(AWorldManager* InWorldManager)
{
	WorldData.Name = AWorldManager::GetWorldInfo().WorldName;
	WorldData.Seed = AWorldManager::GetWorldInfo().WorldSeed;
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
