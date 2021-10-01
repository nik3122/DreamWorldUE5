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

public:
	UVoxelDoor();
	
	//////////////////////////////////////////////////////////////////////////
	// Defaults
public:
	virtual void Initialize(FIndex InIndex, AChunk* InOwner = nullptr) override;

	virtual void LoadData(const FString& InValue) override;

	virtual FString ToData() override;

	virtual void OpenOrClose();

	virtual void OpenTheDoor();

	virtual void CloseTheDoor();

	virtual bool IsOpened() const;

	virtual void SetOpened(bool InValue);

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void OnSpawn_Implementation() override;
	
	virtual void OnTargetHit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetEnter(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetStay(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual void OnTargetExit(ADWCharacter* InTarget, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseDown(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseUp(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual bool OnMouseHold(EMouseButton InMouseButton, const FVoxelHitResult& InHitResult) override;

	virtual void OnMouseHover(const FVoxelHitResult& InHitResult) override;
};
