// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel.generated.h"

class AChunk;

/**
 * ÌåËØ
 */
UCLASS()
class DREAMWORLD_API UVoxel : public UObject
{
	GENERATED_BODY()

public:
	UVoxel();

protected:
	UPROPERTY(BlueprintReadOnly)
	FName VoxelID;

	UPROPERTY(BlueprintReadWrite)
	FIndex Index;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Scale;

	UPROPERTY(BlueprintReadWrite)
	AChunk* Owner;

	UPROPERTY(BlueprintReadWrite)
	AVoxelAuxiliary* Auxiliary;
	
public:
	static UVoxel* Empty();

	static UVoxel* Unknown();

	static UVoxel* NewVoxel(EVoxelType InVoxelType, UObject* InOuter);

	static UVoxel* NewVoxel(const FName& InVoxelID, UObject* InOuter);
	
	static UVoxel* LoadVoxel(FVoxelItem VoxelItem, AChunk* InOwner);

	static UVoxel* LoadVoxel(const FString& InVoxelData, AChunk* InOwner);

	static bool IsValid(UVoxel* InVoxel, bool bCheckOwner = true);

private:
	static UVoxel* EmptyVoxel;

	static UVoxel* UnknownVoxel;

public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr);

	virtual void LoadData(const FString& InVlaue);

	virtual FString ToData();

	virtual bool GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals);

	virtual bool CheckAdjacent(EDirection InDirection);

	virtual bool CheckNeighbors(EVoxelType InVoxelType, bool bIgnoreBottom = false, int InDistance = 1);

	virtual bool OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult);

	virtual bool OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult);

	virtual bool OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult);

	virtual void OnMouseHover(FVoxelHitResult InHitResult);

	virtual void OnGenerate();

	virtual void OnDestroy();

	virtual void OnReplace();

	virtual void OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult);

	virtual void OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult);

	virtual void OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult);

	virtual void OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVoxelData GetVoxelData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetVoxelID() const { return VoxelID; }

	UFUNCTION(BlueprintCallable)
	void SetVoxelID(FName val) { VoxelID = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FIndex GetIndex() const { return Index; }

	UFUNCTION(BlueprintCallable)
	void SetIndex(FIndex val) { Index = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetRotation() const { return Rotation; }

	UFUNCTION(BlueprintCallable)
	void SetRotation(FRotator val) { Rotation = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetScale() const { return Scale; }
	
	UFUNCTION(BlueprintCallable)
	void SetScale(FVector val) { Scale = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AChunk* GetOwner() const { return Owner; }

	UFUNCTION(BlueprintCallable)
	void SetOwner(AChunk* val) { Owner = val; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVoxelAuxiliary* GetAuxiliary() const { return Auxiliary; }

	UFUNCTION(BlueprintCallable)
	void SetAuxiliary(AVoxelAuxiliary* val) { Auxiliary = val; }
};
