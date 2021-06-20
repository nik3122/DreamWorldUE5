// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

class UVoxel;
class APickUp;
class AVitalityObject;
class UVoxelMeshComponent;

/**
 * 体素块
 */
UCLASS()
class DREAMWORLD_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	FIndex Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	int32 Batch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	bool bGenerated;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	UVoxelMeshComponent* SolidMesh;
	
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	UVoxelMeshComponent* SemiMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	UVoxelMeshComponent* TransMesh;
		
	UPROPERTY(BlueprintReadOnly, Category = "ChunkStats")
	TArray<AChunk*> Neighbors;
	
	UPROPERTY(BlueprintReadOnly, Category = "ChunkStats")
	TMap<FIndex, UVoxel*> VoxelMap;
			
	UPROPERTY(BlueprintReadOnly, Category = "ChunkStats")
	TArray<ADWCharacter*> Characters;

	UPROPERTY(BlueprintReadOnly, Category = "ChunkStats")
	TArray<AVitalityObject*> VitalityObjects;
	
	UPROPERTY(BlueprintReadOnly, Category = "ChunkStats")
	TArray<APickUp*> PickUps;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(FIndex InIndex, int32 InBatch);

	void BuildMap();

	void LoadMap(FChunkData InChunkData);

	void LoadActors(FChunkData InChunkData);

	void GenerateMesh();
		
	void OnGenerated();

	void OnDestroy();
		
	void UpdateNeighbors(FIndex InIndex);

	void UpdateNeighbors(int InX, int InY, int InZ);

	UVoxel* GetVoxel(FIndex InIndex);

	UVoxel* GetVoxel(int InX, int InY, int InZ);

	bool CheckVoxel(FIndex InIndex, FVector InRange = FVector::OneVector, bool bIgnoreTransparent = true);

	bool CheckVoxel(int InX, int InY, int InZ, FVector InRange = FVector::OneVector, bool bIgnoreTransparent = true);

	bool SetVoxelSample(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh = false);

	bool SetVoxelSample(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh = false);

	bool SetVoxelComplex(FIndex InIndex, UVoxel* InVoxel, bool bGenerateMesh = false);

	bool SetVoxelComplex(int InX, int InY, int InZ, UVoxel* InVoxel, bool bGenerateMesh = false);

	bool GenerateVoxel(FIndex InIndex, UVoxel* InVoxel);

	bool DestroyVoxel(FIndex InIndex);

	bool DestroyVoxel(UVoxel* InVoxel);

	bool ReplaceVoxel(UVoxel* InOldVoxel, UVoxel* InNewVoxel);

	APickUp* SpawnPickUp(FItem InItem, FVector InLocation);

	APickUp* SpawnPickUp(FPickUpData InPickUpData);

	ADWCharacter* SpawnCharacter(FCharacterData InSaveData);

	AVitalityObject* SpawnVitalityObject(FVitalityObjectData InSaveData);

	void DestroyPickUp(APickUp* InPickUp);

	void DestroyCharacter(ADWCharacter* InCharacter);

	void DestroyVitalityObject(AVitalityObject* InVitalityObject);

	void AttachPickUp(APickUp* InPickUp);

	void AttachCharacter(ADWCharacter* InCharacter);

	void AttachVitalityObject(AVitalityObject* InVitalityObject);

	void DetachPickUp(APickUp* InPickUp);

	void DetachCharacter(ADWCharacter* InCharacter);

	void DetachVitalityObject(AVitalityObject* InVitalityObject);

	bool IsOnTheChunk(FIndex InIndex);

	bool IsOnTheChunk(FVector InLocation);

	FIndex LocationToIndex(FVector InLocation, bool bWorldSpace = true);

	FVector IndexToLocation(FIndex InIndex, bool bWorldSpace = true);

	FIndex LocalIndexToWorld(FIndex InIndex);

	FIndex WorldIndexToLocal(FIndex InIndex);

protected:
	bool GenerateMap();

	void GetNeighbors();

	void BreakNeighbors();

public:
	FIndex GetIndex() const { return Index; }

	int32 GetBatch() const { return Batch; }
	
	bool IsGenerated() const { return bGenerated; }
	
	UVoxelMeshComponent* GetSolidMesh();

	UVoxelMeshComponent* GetSemiMesh();

	UVoxelMeshComponent* GetTransMesh();
};
