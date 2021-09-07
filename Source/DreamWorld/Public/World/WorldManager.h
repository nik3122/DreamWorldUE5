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

	FORCEINLINE FVector GetBlockSizedNormal(FVector InNormal, float InLength = 0.25f) const
	{
		return BlockSize * InNormal * InLength;
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
	FWorldInfo WorldInfo;

	FRandomStream RandomStream;

public:
	static FWorldInfo& GetInfo()
	{
		return Current ? Current->WorldInfo : FWorldInfo::Empty;
	}
	
	UFUNCTION(BlueprintPure)
	FRandomStream GetRandomStream() const { return RandomStream; }
			
	UFUNCTION(BlueprintCallable)
	void InitRandomStream(int32 InDeltaSeed);

public:
	void LoadWorld();

	void UnloadWorld();

	//////////////////////////////////////////////////////////////////////////
	// Chunk
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TMap<FIndex, AChunk*> ChunkMap;
	
private:
	bool bBasicGenerated;

	int32 ChunkSpawnBatch;

	FIndex LastGenerateIndex;

	FIndex LastStayChunkIndex;

	TArray<FIndex> ChunkSpawnQueue;

	TArray<AChunk*> ChunkMapBuildQueue;

	TArray<AChunk*> ChunkGenerateQueue;

	TArray<AChunk*> ChunkDestroyQueue;

protected:
	void GenerateTerrain();

	void OnBasicGenerated(FVector InPlayerLocation);

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
	bool IsBasicGenerated() const { return bBasicGenerated; }

	UFUNCTION(BlueprintPure)
	int GetChunkNum(bool bNeedGenerated = false);

	EVoxelType GetNoiseVoxelType(FIndex InIndex);

	FVoxelData GetNoiseVoxelData(FIndex InIndex);

	int GetNoiseTerrainHeight(FVector InOffset, FVector InScale);
		
	FIndex LocationToChunkIndex(FVector InLocation, bool bIgnoreZ = false);

	FVector ChunkIndexToLocation(FIndex InIndex);

	bool ChunkTraceSingle(AChunk* InChunk, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool ChunkTraceSingle(FVector RayStart, FVector RayEnd, float InRadius, float InHalfHeight, FHitResult& OutHitResult);

	bool VoxelTraceSingle(UVoxel* InVoxel, FVector InPoint, FHitResult& OutHitResult);

	//////////////////////////////////////////////////////////////////////////
	// Team
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	TMap<FName, FTeamData> TeamMap;

public:
	bool IsExistTeam(const FName& InTeamID);

	bool CreateTeam(ADWCharacter* InCaptain, FName InTeamName = NAME_None, FString InTeamDetail = TEXT(""));

	bool DissolveTeam(const FName& InTeamID, ADWCharacter* InCaptain = nullptr);

	FTeamData* GetTeamData(const FName& InTeamID);
};
