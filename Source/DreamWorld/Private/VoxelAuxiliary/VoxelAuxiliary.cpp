// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelAuxiliary.h"
#include "Voxel/Voxel.h"
#include "World/Chunk.h"

// Sets default values
AVoxelAuxiliary::AVoxelAuxiliary()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	VoxelIndex = FIndex::ZeroIndex;
}

// Called when the game starts or when spawned
void AVoxelAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelAuxiliary::Initialize(AChunk* InOwnerChunk, FIndex InVoxelIndex)
{
	VoxelIndex = InVoxelIndex;
}

FVoxelItem& AVoxelAuxiliary::GetVoxelItem()
{
	if(OwnerChunk)
	{
		return OwnerChunk->GetVoxelItem(VoxelIndex);
	}
	return FVoxelItem::EmptyVoxel;
}
