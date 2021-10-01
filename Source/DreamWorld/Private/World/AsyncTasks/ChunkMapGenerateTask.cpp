// Fill out your copyright notice in the Description page of Project Settings.


#include "World/AsyncTasks/ChunkMapGenerateTask.h"

#include "WorldManager.h"

ChunkMapGenerateTask::ChunkMapGenerateTask(AWorldManager* InWorldManager, TArray<AChunk*> InChunkMapGenerateQueue)
{
	WorldManager = InWorldManager;
	ChunkMapGenerateQueue = InChunkMapGenerateQueue;
}

void ChunkMapGenerateTask::DoWork()
{
	for(int32 i = 0; i < ChunkMapGenerateQueue.Num(); i++)
	{
		WorldManager->GenerateChunkMap(ChunkMapGenerateQueue[i]);
	}
}
