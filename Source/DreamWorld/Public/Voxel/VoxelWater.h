// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel/Voxel.h"
#include "VoxelWater.generated.h"

class AChunk;

/**
 * ÌåËØË®
 */
UCLASS()
class DREAMWORLD_API UVoxelWater : public UVoxel
{
	GENERATED_BODY()

public:
	UVoxelWater();
	
public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr) override;

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
