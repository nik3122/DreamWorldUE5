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
	FWorldSaveData WorldData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FVector, FChunkData> ChunkDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FPlayerRecordSaveData> PlayerRecordDatas;

public:
	UFUNCTION(BlueprintCallable)
	void RefreshWorldData();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FWorldSaveData GetWorldData() const { return WorldData; }

	UFUNCTION(BlueprintCallable)
	void SetWorldData(FWorldSaveData InWorldData) { WorldData = InWorldData; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsExistPlayerRecord(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable)
	void SavePlayerRecord(FPlayerRecordSaveData InPlayerRecordData);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerRecordSaveData LoadPlayerRecord(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerRecord(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsExistChunkData(FIndex InChunkIndex);

	UFUNCTION(BlueprintCallable)
	void SaveChunkData(FIndex InChunkIndex, FChunkData InChunkData);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FChunkData LoadChunkData(FIndex InChunkIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FVector, FChunkData>& GetChunkDatas() { return ChunkDatas; }
};
