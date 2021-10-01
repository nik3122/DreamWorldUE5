// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "ObjectPoolInterface.h"

#include "Voxel.generated.h"

class AChunk;

/**
 * ����
 */
UCLASS()
class DREAMWORLD_API UVoxel : public UObject, public IObjectPoolInterface
{
	GENERATED_BODY()

public:
	UVoxel();
	
	//////////////////////////////////////////////////////////////////////////
	// Statics
public:
	static UVoxel* EmptyVoxel;

	static UVoxel* UnknownVoxel;

public:
	static UVoxel* SpawnVoxel(UObject* InWorldContext, EVoxelType InVoxelType);

	static UVoxel* SpawnVoxel(UObject* InWorldContext, const FName& InVoxelID);

	static UVoxel* LoadVoxel(AChunk* InOwner, const FVoxelItem& InVoxelItem);

	static UVoxel* LoadVoxel(AChunk* InOwner, const FString& InVoxelData);
	
	static void DespawnVoxel(UObject* InWorldContext, UVoxel* InVoxel);

	static bool IsValid(UVoxel* InVoxel, bool bCheckOwner = true);

	//////////////////////////////////////////////////////////////////////////
	// Defaults
public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr);

	virtual void LoadData(const FString& InValue);

	virtual void LoadItem(const FVoxelItem& InVoxelItem);

	virtual FString ToData();

	virtual FVoxelItem ToItem();

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void OnSpawn_Implementation() override;

	virtual void OnDespawn_Implementation() override;

	virtual void OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult);

	virtual void OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult);

	virtual void OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult);

	virtual void OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult);

	virtual bool OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult);

	virtual bool OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult);

	virtual bool OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult);

	virtual void OnMouseHover(const FVoxelHitResult& InHitResult);

	//////////////////////////////////////////////////////////////////////////
	// Stats
protected:
	UPROPERTY(BlueprintReadOnly)
	FName ID;

	UPROPERTY(BlueprintReadWrite)
	FIndex Index;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Scale;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FParameter> Params;

	UPROPERTY(BlueprintReadWrite)
	AChunk* Owner;

	UPROPERTY(BlueprintReadWrite)
	AVoxelAuxiliary* Auxiliary;

public:
	UFUNCTION(BlueprintPure)
	FVoxelData GetVoxelData() const;

	UFUNCTION(BlueprintPure)
	FName GetID() const { return ID; }

	UFUNCTION(BlueprintCallable)
	void SetID(FName val) { ID = val; }

	UFUNCTION(BlueprintPure)
	FIndex GetIndex() const { return Index; }

	UFUNCTION(BlueprintCallable)
	void SetIndex(FIndex val) { Index = val; }

	UFUNCTION(BlueprintPure)
	FRotator GetRotation() const { return Rotation; }

	UFUNCTION(BlueprintCallable)
	void SetRotation(FRotator val) { Rotation = val; }

	UFUNCTION(BlueprintPure)
	FVector GetScale() const { return Scale; }
	
	UFUNCTION(BlueprintCallable)
	void SetScale(FVector val) { Scale = val; }

	UFUNCTION(BlueprintPure)
	AChunk* GetOwner() const { return Owner; }

	UFUNCTION(BlueprintCallable)
	void SetOwner(AChunk* val) { Owner = val; }

	UFUNCTION(BlueprintPure)
	AVoxelAuxiliary* GetAuxiliary() const { return Auxiliary; }

	UFUNCTION(BlueprintCallable)
	void SetAuxiliary(AVoxelAuxiliary* val) { Auxiliary = val; }
};
