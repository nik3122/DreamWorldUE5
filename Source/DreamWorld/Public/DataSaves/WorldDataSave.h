// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/SaveGame.h"
#include "WorldDataSave.generated.h"

/**
 * 世界数据存取类
 */
UCLASS()
class UWorldDataSave : public USaveGame
{
	GENERATED_BODY()

public:
	UWorldDataSave();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FWorldData WorldData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FVector, FChunkData> ChunkDatas;

public:
	UFUNCTION(BlueprintCallable)
	void SaveWorldData(AWorldManager* InWorldManager);

	UFUNCTION(BlueprintCallable)
	bool IsExistChunkData(FIndex InChunkIndex);

	UFUNCTION(BlueprintCallable)
	void SaveChunkData(FIndex InChunkIndex, FChunkData InChunkData);
	
	UFUNCTION(BlueprintCallable)
	FChunkData LoadChunkData(FIndex InChunkIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FWorldData GetWorldData() const { return WorldData; }

	UFUNCTION(BlueprintCallable)
	void SetWorldData(FWorldData InWorldData) { WorldData = InWorldData; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FVector, FChunkData> GetChunkDatas() const { return ChunkDatas; }
};
