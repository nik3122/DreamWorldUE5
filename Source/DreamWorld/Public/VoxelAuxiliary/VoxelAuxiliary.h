// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "VoxelAuxiliary.generated.h"

class UVoxel;

/**
 * ���ظ�����
 */
UCLASS()
class DREAMWORLD_API AVoxelAuxiliary : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelAuxiliary();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Initialize(AChunk* InOwnerChunk, FIndex InVoxelIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIndex VoxelIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AChunk* OwnerChunk;

public:
	FIndex GetVoxelIndex() const { return VoxelIndex; }

	FVoxelItem& GetVoxelItem();
};
