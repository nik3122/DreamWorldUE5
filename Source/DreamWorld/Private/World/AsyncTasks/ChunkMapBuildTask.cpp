// Fill out your copyright notice in the Description page of Project Settings.


#include "World/AsyncTasks/ChunkMapBuildTask.h"

#include "WorldManager.h"

ChunkMapBuildTask::ChunkMapBuildTask(AWorldManager* InWorldManager, TArray<AChunk*> InChunkMapBuildQueue)
{
	WorldManager = InWorldManager;
	ChunkMapBuildQueue = InChunkMapBuildQueue;
}

void ChunkMapBuildTask::DoWork()
{
	for(int32 i = 0; i < ChunkMapBuildQueue.Num(); i++)
	{
		WorldManager->BuildChunkMap(ChunkMapBuildQueue[i]);
	}
}
