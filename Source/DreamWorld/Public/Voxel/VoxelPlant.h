// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Voxel/Voxel.h"
#include "VoxelPlant.generated.h"

class AChunk;

/**
 * ����ֲ��
 */
UCLASS()
class DREAMWORLD_API UVoxelPlant : public UVoxel
{
	GENERATED_BODY()

public:
	UVoxelPlant();

	
	//////////////////////////////////////////////////////////////////////////
	// Defaults
public:
	virtual void LoadData(const FString& InValue) override;

	virtual FString ToData() override;

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetEnter(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetStay(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetExit(UDWCharacterPart* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual void OnMouseHover(const FVoxelHitResult& InHitResult) override;
};
