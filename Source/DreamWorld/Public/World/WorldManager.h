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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FString WorldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 WorldSeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 BlockSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkHightRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkSpawnRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkSpawnDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkSpawnSpeed;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkDestroySpeed;
					
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkMapBuildSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int32 ChunkGenerateSpeed;
						
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float VitalityRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float CharacterRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float TerrainBaseHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FVector TerrainPlainScale;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FVector TerrainMountainScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FVector TerrainStoneVoxelScale;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FVector TerrainSandVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float TerrainWaterVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float TerrainBedrockVoxelScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<FChunkMaterial> ChunkMaterials;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UWorldTimerComponent* WorldTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UWorldWeatherComponent* WorldWeather;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USceneCaptureComponent2D* VoxelsCapture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* BoundsMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldStats")
	bool bBasicGenerated;

	UPROPERTY(BlueprintReadOnly, Category = "WorldStats")
	TMap<FIndex, AChunk*> ChunkMap;

	UPROPERTY(BlueprintReadOnly, Category = "WorldStats")
	TMap<FName, FTeamData> TeamMap;

private:
	int32 ChunkSpawnBatch;

	FIndex LastGenerateIndex;

	FIndex LastStayChunkIndex;

	FRandomStream RandomStream;

	TArray<FIndex> ChunkSpawnQueue;

	TArray<AChunk*> ChunkMapBuildQueue;

	TArray<AChunk*> ChunkGenerateQueue;

	TArray<AChunk*> ChunkDestroyQueue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LoadWorld();

	void UnloadWorld();

	void GenerateTerrain();

	void OnBasicGenerated(FVector InPlayerLocation);

	void GeneratePreviews();

	void GenerateChunks(FIndex InIndex);

	void SpawnChunk(FIndex InIndex, bool bAddToQueue = true);

	void BuildChunkMap(AChunk* InChunk);

	void GenerateChunk(AChunk* InChunk);

	void DestroyChunk(AChunk* InChunk);

	void AddToSpawnQueue(FIndex InIndex);

	void AddToBuildMapQueue(AChunk* InChunk);

	void AddToGenerateQueue(AChunk* InChunk);

	void AddToDestroyQueue(AChunk* InChunk);

	EVoxelType GetNoiseVoxelType(FIndex InIndex);

	FVoxelData GetNoiseVoxelData(FIndex InIndex);

	int GetNoiseTerrainHeight(FVector InOffset, FVector InScale);

	FTeamData* GetTeamData(const FName& InTeamID);

	bool IsExistTeam(const FName& InTeamID);

	bool CreateTeam(ADWCharacter* InCaptain, FName InTeamName = NAME_None, FString InTeamDetail = TEXT(""));

	bool DissolveTeam(const FName& InTeamID, ADWCharacter* InCaptain = nullptr);

	FChunkMaterial GetChunkMaterial(ETransparency Transparency);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetChunkLength();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetWorldLength();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWorldHeight();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkDistance();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetNumChunks(bool bNeedGenerated = false);

	FVector GetBlockSizedNormal(FVector InNormal, float InLength = 0.25f);

	ETraceTypeQuery GetGameTrace(EGameTraceType GameTraceType);

	AChunk* FindChunk(FVector InLocation);

	AChunk* FindChunk(FIndex InIndex);
		
	FIndex LocationToChunkIndex(FVector InLocation, bool bIgnoreZ = false);

	FVector ChunkIndexToLocation(FIndex InIndex);

	bool ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool VoxelTraceSingle(UVoxel* InVoxel, FVector InPoint, FHitResult& OutHitResult);
			
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRandomStream GetRandomStream() const { return RandomStream; }
			
	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 InDeltaSeed);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UWorldTimerComponent* GetWorldTimer() const { return WorldTimer; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetWorldName() const { return WorldName; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWorldSeed() const { return WorldSeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetBlockSize() const { return BlockSize; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkSize() const { return ChunkSize; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkHightRange() const { return ChunkHightRange; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkSpawnRange() const { return ChunkSpawnRange; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkSpawnDistance() const { return ChunkSpawnDistance; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkSpawnSpeed() const { return ChunkSpawnSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkDestroySpeed() const { return ChunkDestroySpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkMapBuildSpeed() const { return ChunkMapBuildSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetChunkGenerateSpeed() const { return ChunkGenerateSpeed; }
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetVitalityRaceDensity() const { return VitalityRateDensity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCharacterRaceDensity() const { return CharacterRateDensity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTerrainBaseHeight() const { return TerrainBaseHeight; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTerrainPlainScale() const { return TerrainPlainScale; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTerrainMountainScale() const { return TerrainMountainScale; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTerrainStoneVoxelScale() const { return TerrainStoneVoxelScale; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTerrainSandVoxelScale() const { return TerrainSandVoxelScale; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTerrainWaterVoxelScale() const { return TerrainWaterVoxelScale; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTerrainBedrockVoxelScale() const { return TerrainBedrockVoxelScale; }
};
