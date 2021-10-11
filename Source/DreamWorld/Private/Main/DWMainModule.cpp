// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWMainModule.h"

#include "Chunk.h"
#include "ObjectPoolModuleBPLibrary.h"
#include "SpawnPoolModuleBPLibrary.h"
#include "Voxel.h"
#include "VoxelDoor.h"
#include "VoxelPlant.h"
#include "VoxelTorch.h"
#include "VoxelWater.h"

ADWMainModule::ADWMainModule()
{
	// set default pawn class to our Blueprinted character
	
}

void ADWMainModule::InitializeModules_Implementation()
{
	Super::InitializeModules_Implementation();
	
	UVoxel::EmptyVoxel = UVoxel::SpawnVoxel(EVoxelType::Empty);
	UVoxel::UnknownVoxel = UVoxel::SpawnVoxel(EVoxelType::Unknown);
	
	USpawnPoolModuleBPLibrary::DespawnActor(USpawnPoolModuleBPLibrary::SpawnActor<AChunk>());
}
