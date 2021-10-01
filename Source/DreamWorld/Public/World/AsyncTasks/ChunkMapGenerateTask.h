// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
* 
*/
class ChunkMapGenerateTask : public FNonAbandonableTask
{
	friend class FAsyncTask<ChunkMapGenerateTask>;

public:
	class AWorldManager* WorldManager;
	TArray<class AChunk*> ChunkMapGenerateQueue;

	ChunkMapGenerateTask(class AWorldManager* InWorldManager, TArray<class AChunk*> InChunkMapGenerateQueue);

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMapGenerateTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};
