// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DataSave/WorldDataSave.h"
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
}

bool UWorldDataSave::IsExistChunkData(FIndex InChunkIndex)
{
	return ChunkDatas.Contains(InChunkIndex.ToVector());
}

void UWorldDataSave::SaveWorldData(const int32 InUserIndex)
{
	WorldData.Name = AWorldManager::GetInfo().WorldName;
	WorldData.Seed = AWorldManager::GetInfo().WorldSeed;
	
	UDWHelper::Debug(FString::Printf(TEXT("Saving world : %s, %dchunks."), *GetWorldData().Name, GetChunkDatas().Num()), EDebugType::Console);
	UGameplayStatics::SaveGameToSlot(this, TEXT("World_") + GetWorldData().Name, InUserIndex);
}

void UWorldDataSave::RemoveWorldData(const int32 InUserIndex)
{
	UGameplayStatics::DeleteGameInSlot(TEXT("World_") + GetWorldData().Name, InUserIndex);
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
