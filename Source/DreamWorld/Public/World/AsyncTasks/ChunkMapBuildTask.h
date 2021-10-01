// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ChunkMapBuildTask : public FNonAbandonableTask
{
	friend class FAsyncTask<ChunkMapBuildTask>;
	
	class AWorldManager* WorldManager;
	TArray<class AChunk*> ChunkMapBuildQueue;

	ChunkMapBuildTask(class AWorldManager* InWorldManager, TArray<class AChunk*> InChunkMapBuildQueue);

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMapBuildTask, STATGROUP_ThreadPoolAsyncTasks);
	}

public:
	TArray<class AChunk*> GetQueue() const { return ChunkMapBuildQueue; }
};
