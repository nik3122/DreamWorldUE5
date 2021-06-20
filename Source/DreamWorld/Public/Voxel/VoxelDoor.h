// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel/Voxel.h"
#include "VoxelDoor.generated.h"

class AChunk;

/**
 * ÃÂÀÿ√≈
 */
UCLASS()
class DREAMWORLD_API UVoxelDoor : public UVoxel
{
	GENERATED_BODY()

protected:
	bool bClosed;

public:
	UVoxelDoor();
	
public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr) override;

	virtual void LoadData(const FString& InVlaue) override;

	virtual FString ToData() override;

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

	virtual void OpenOrClose();

	virtual void OpenTheDoor();

	virtual void CloseTheDoor();
};
