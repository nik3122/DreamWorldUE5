// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel/Voxel.h"
#include "VoxelPlant.generated.h"

class AChunk;

/**
 * ÃÂÀÿ÷≤ŒÔ
 */
UCLASS()
class DREAMWORLD_API UVoxelPlant : public UVoxel
{
	GENERATED_BODY()

public:
	UVoxelPlant();

public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr) override;

	virtual bool GetMeshDatas(TArray<FVector>& OutMeshVertices, TArray<FVector>& OutMeshNormals) override;

	virtual bool CheckAdjacent(EDirection InDirection) override;

	virtual bool OnMouseDown(EMouseButton InMouseButton, FVoxelHitResult InHitResult) override;

	virtual bool OnMouseUp(EMouseButton InMouseButton, FVoxelHitResult InHitResult) override;

	virtual bool OnMouseHold(EMouseButton InMouseButton, FVoxelHitResult InHitResult) override;

	virtual void OnMouseHover(FVoxelHitResult InHitResult) override;

	virtual void OnGenerate() override;

	virtual void OnDestroy() override;

	virtual void OnReplace() override;

	virtual void OnTargetHit(ADWCharacter* InTarget, FVoxelHitResult InHitResult) override;

	virtual void OnTargetEnter(ADWCharacter* InTarget, FVoxelHitResult InHitResult) override;

	virtual void OnTargetStay(ADWCharacter* InTarget, FVoxelHitResult InHitResult) override;

	virtual void OnTargetExit(ADWCharacter* InTarget, FVoxelHitResult InHitResult) override;
};
