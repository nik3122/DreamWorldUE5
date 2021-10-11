// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "ChunkMapBuildTask.h"
#include "ChunkMapGenerateTask.h"
#include "WorldManager.generated.h"

class AChunk;
class ADWCharacter;
class UWorldDataSave;
class UPlayerDataSave;
class UWorldTimerComponent;
class USceneCaptureComponent2D;
class UWorldWeatherComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBasicGenerated, FVector, InPlayerLocation);

/**
 * 世界管理器
 */
UCLASS(hidecategories = (Tick, Replication, Rendering, Collision, Actor, Input, LOD, Cooking, Hidden, WorldPartition, Hlod))
class DREAMWORLD_API AWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////////////////////////////////
	// Instance
protected:
	static AWorldManager* Current;

public:
	static AWorldManager* Get() { return Current; }

protected:
	static FWorldSaveData WorldData;

	static UWorldDataSave* DataSave;

public:
	static FWorldSaveData& GetWorldData() { return WorldData; }

	static UWorldDataSave* GetDataSave() { return DataSave; }

	static EVoxelType GetNoiseVoxelType(FIndex InIndex);

	static FVoxelData GetNoiseVoxelData(FIndex InIndex);

	static int GetNoiseTerrainHeight(FVector InOffset, FVector InScale);
		
	static FIndex LocationToChunkIndex(FVector InLocation, bool bIgnoreZ = false);

	static FVector ChunkIndexToLocation(FIndex InIndex);

	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UWorldTimerComponent* WorldTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UWorldWeatherComponent* WorldWeather;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneCaptureComponent2D* VoxelsCapture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BoundsMesh;

public:
	UFUNCTION(BlueprintPure)
	UWorldTimerComponent* GetWorldTimer() const { return WorldTimer; }

	//////////////////////////////////////////////////////////////////////////
	// World
protected:
	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkSpawnRange;

	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkSpawnDistance;

	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkSpawnSpeed;
		
	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkDestroySpeed;
					
	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkMapBuildSpeed;
					
	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkMapGenerateSpeed;

	UPROPERTY(EditAnywhere, Category = "World")
	int32 ChunkGenerateSpeed;

	UPROPERTY(EditAnywhere, Category = "World")
	float BasicPercentage;

	UPROPERTY(VisibleAnywhere, Category = "World")
	bool bBasicGenerated;

	FRandomStream RandomStream;

public:
	UPROPERTY(BlueprintAssignable)
	FOnBasicGenerated OnBasicGenerated;

public:
	void LoadWorld(const FString& InWorldName);

	void UnloadWorld();

public:
	UFUNCTION(BlueprintPure)
	bool IsBasicGenerated() const { return bBasicGenerated; }
	
	UFUNCTION(BlueprintPure)
	FRandomStream GetRandomStream() const { return RandomStream; }
			
	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 InDeltaSeed);

	//////////////////////////////////////////////////////////////////////////
	// Chunk
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chunk")
	TMap<FIndex, AChunk*> ChunkMap;
	
private:
	int32 ChunkSpawnBatch;

	FIndex LastGenerateIndex;

	FIndex LastStayChunkIndex;

	TArray<FIndex> ChunkSpawnQueue;

	TArray<AChunk*> ChunkMapBuildQueue;

	TArray<AChunk*> ChunkMapGenerateQueue;

	TArray<AChunk*> ChunkGenerateQueue;

	TArray<AChunk*> ChunkDestroyQueue;
	
	TArray<FAsyncTask<ChunkMapBuildTask>*> ChunkMapBuildTasks;
	
	TArray<FAsyncTask<ChunkMapGenerateTask>*> ChunkMapGenerateTasks;

protected:
	UFUNCTION()
	void OnPlayerSpawned(ADWPlayerCharacter* InPlayerCharacter);

	void GeneratePreviews();

	void GenerateTerrain();

	void GenerateChunks(FIndex InIndex);

public:
	void BuildChunkMap(AChunk* InChunk);

	void GenerateChunkMap(AChunk* InChunk);

	void GenerateChunk(AChunk* InChunk);

	void DestroyChunk(AChunk* InChunk);

	void AddToSpawnQueue(FIndex InIndex);

	void AddToMapBuildQueue(AChunk* InChunk);

	void AddToMapGenerateQueue(AChunk* InChunk);

	void AddToGenerateQueue(AChunk* InChunk);

	void AddToDestroyQueue(AChunk* InChunk);

public:
	AChunk* SpawnChunk(FIndex InIndex, bool bAddToQueue = true);

	AChunk* FindChunk(FVector InLocation);

	AChunk* FindChunk(FIndex InIndex);

public:
	int GetChunkNum(bool bNeedGenerated = false) const;

	float GetWorldLength() const;

	int32 GetChunkDistance() const;

	bool ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool VoxelTraceSingle(const FVoxelItem& InVoxelItem, FVector InPoint, FHitResult& OutHitResult);

	//////////////////////////////////////////////////////////////////////////
	// Team
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	TMap<FName, FTeamData> TeamMap;

public:
	bool IsExistTeam(const FName& InTeamID) const;

	bool CreateTeam(ADWCharacter* InCaptain, FName InTeamName = NAME_None, FString InTeamDetail = TEXT(""));

	bool DissolveTeam(const FName& InTeamID, ADWCharacter* InCaptain = nullptr);

	FTeamData* GetTeamData(const FName& InTeamID);
};
