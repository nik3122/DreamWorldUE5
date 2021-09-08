// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
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
UCLASS()
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FWorldData WorldData;

	bool bBasicGenerated;

	FRandomStream RandomStream;

	static UWorldDataSave* DataSave;

public:
	UPROPERTY(BlueprintAssignable)
	FOnBasicGenerated OnBasicGenerated;

public:
	void LoadWorld(const FString& InWorldName);

	void UnloadWorld();

public:
	static FWorldData& GetData()
	{
		return Current ? Current->WorldData : FWorldData::Empty;
	}

	static UWorldDataSave* GetDataSave() { return DataSave; }

	UFUNCTION(BlueprintPure)
	bool IsBasicGenerated() const { return bBasicGenerated; }
	
	UFUNCTION(BlueprintPure)
	FRandomStream GetRandomStream() const { return RandomStream; }
			
	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 InDeltaSeed);

	//////////////////////////////////////////////////////////////////////////
	// Chunk
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TMap<FIndex, AChunk*> ChunkMap;
	
private:
	int32 ChunkSpawnBatch;

	FIndex LastGenerateIndex;

	FIndex LastStayChunkIndex;

	TArray<FIndex> ChunkSpawnQueue;

	TArray<AChunk*> ChunkMapBuildQueue;

	TArray<AChunk*> ChunkGenerateQueue;

	TArray<AChunk*> ChunkDestroyQueue;

protected:
	void GenerateTerrain();

	UFUNCTION()
	void OnPlayerSpawned(ADWPlayerCharacter* InPlayerCharacter);

	void GeneratePreviews();

	void GenerateChunks(FIndex InIndex);

	void BuildChunkMap(AChunk* InChunk);

	void GenerateChunk(AChunk* InChunk);

	void DestroyChunk(AChunk* InChunk);

	void AddToSpawnQueue(FIndex InIndex);

	void AddToBuildMapQueue(AChunk* InChunk);

	void AddToGenerateQueue(AChunk* InChunk);

	void AddToDestroyQueue(AChunk* InChunk);

public:
	AChunk* SpawnChunk(FIndex InIndex, bool bAddToQueue = true);

	AChunk* FindChunk(FVector InLocation);

	AChunk* FindChunk(FIndex InIndex);

public:
	UFUNCTION(BlueprintPure)
	int GetChunkNum(bool bNeedGenerated = false) const;

	EVoxelType GetNoiseVoxelType(FIndex InIndex) const;

	FVoxelData GetNoiseVoxelData(FIndex InIndex);

	int GetNoiseTerrainHeight(FVector InOffset, FVector InScale) const;
		
	FIndex LocationToChunkIndex(FVector InLocation, bool bIgnoreZ = false) const;

	FVector ChunkIndexToLocation(FIndex InIndex) const;

	bool ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool VoxelTraceSingle(UVoxel* InVoxel, FVector InPoint, FHitResult& OutHitResult);

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
