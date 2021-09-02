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

USTRUCT(BlueprintType)
struct FWorldInfo
{
	GENERATED_BODY()

public:
	FORCEINLINE FWorldInfo()
	{
		WorldName = TEXT("");
		WorldSeed = 0;

		BlockSize = 80;
		ChunkSize = 16;
		ChunkHeightRange = 3;
		ChunkSpawnRange = 5;
		ChunkSpawnDistance = 2;

		ChunkSpawnSpeed = 100;
		ChunkDestroySpeed = 100;
		ChunkMapBuildSpeed = 5;
		ChunkGenerateSpeed = 1;

		VitalityRateDensity = 0.2f;
		CharacterRateDensity = 0.2f;

		TerrainBaseHeight = 0.1f;
		TerrainPlainScale = FVector(0.005f, 0.005f, 0.2f);
		TerrainMountainScale = FVector(0.03f, 0.03f, 0.25f);
		TerrainStoneVoxelScale = FVector(0.05f, 0.05f, 0.18f);
		TerrainSandVoxelScale = FVector(0.04f, 0.04f, 0.21f);
		TerrainWaterVoxelScale = 0.3f;
		TerrainBedrockVoxelScale = 0.01f;

		ChunkMaterials = TArray<FChunkMaterial>();
	}

public:
	static FWorldInfo Empty;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WorldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WorldSeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkHeightRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnSpeed;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkDestroySpeed;
					
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkMapBuildSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkGenerateSpeed;
						
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VitalityRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CharacterRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainBaseHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainPlainScale;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainMountainScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainStoneVoxelScale;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainSandVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainWaterVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainBedrockVoxelScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChunkMaterial> ChunkMaterials;

public:
	FORCEINLINE float GetChunkLength() const
	{
		return ChunkSize * BlockSize;
	}

	FORCEINLINE float GetWorldLength() const
	{
		return ChunkSize * ChunkSpawnRange * 2;
	}

	FORCEINLINE int32 GetWorldHeight() const
	{
		return ChunkSize * ChunkHeightRange;
	}

	FORCEINLINE int32 GetChunkDistance() const
	{
		return ChunkSpawnRange + ChunkSpawnDistance;
	}

	FORCEINLINE FChunkMaterial GetChunkMaterial(ETransparency Transparency) const
	{
		return ChunkMaterials[FMath::Clamp((int32)Transparency, 0, ChunkMaterials.Num())];
	}
};

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
	static AWorldManager* Current;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldInfo WorldInfo;

public:
	static AWorldManager* GetCurrent() { return Current; }

	static FWorldInfo& GetWorldInfo() { return Current ? Current->WorldInfo : FWorldInfo::Empty; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UWorldTimerComponent* WorldTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UWorldWeatherComponent* WorldWeather;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USceneCaptureComponent2D* VoxelsCapture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* BoundsMesh;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chunk")
	TMap<FIndex, AChunk*> ChunkMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	TMap<FName, FTeamData> TeamMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bBasicGenerated;

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

	UFUNCTION(BlueprintPure)
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
			
	UFUNCTION(BlueprintPure)
	FRandomStream GetRandomStream() const { return RandomStream; }
			
	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 InDeltaSeed);

	UFUNCTION(BlueprintPure)
	FORCEINLINE UWorldTimerComponent* GetWorldTimer() const { return WorldTimer; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WorldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WorldSeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkHightRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSpawnSpeed;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkDestroySpeed;
					
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkMapBuildSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkGenerateSpeed;
						
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VitalityRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CharacterRateDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainBaseHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainPlainScale;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainMountainScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainStoneVoxelScale;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TerrainSandVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainWaterVoxelScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TerrainBedrockVoxelScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChunkMaterial> ChunkMaterials;
};
