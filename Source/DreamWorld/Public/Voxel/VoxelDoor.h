// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel/Voxel.h"
#include "VoxelDoor.generated.h"

class AChunk;

/**
 * ������
 */
UCLASS()
class DREAMWORLD_API UVoxelDoor : public UVoxel
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	bool bClosed;

public:
	UVoxelDoor();
	
	//////////////////////////////////////////////////////////////////////////
	// Defaults
public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr) override;

	virtual void LoadData(const FString& InValue) override;

	virtual FString ToData() override;

	virtual void OnGenerate() override;

	virtual void OnDestroy() override;

	virtual void OnReplace() override;

	virtual void OnDespawn_Implementation() override;

	virtual bool GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals) override;

	virtual bool CheckAdjacent(EDirection InDirection) override;

	virtual void OpenOrClose();

	virtual void OpenTheDoor();

	virtual void CloseTheDoor();

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual void OnMouseHover(const FVoxelHitResult& InHitResult) override;
};
