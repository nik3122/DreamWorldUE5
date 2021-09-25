// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DWMainModule.h"

#include "Voxel.h"

ADWMainModule::ADWMainModule()
{
	// set default pawn class to our Blueprinted character
	
}

void ADWMainModule::InitializeModules_Implementation()
{
	Super::InitializeModules_Implementation();
	
	UVoxel::EmptyVoxel = UVoxel::SpawnVoxel(this, EVoxelType::Empty);
	UVoxel::UnknownVoxel = UVoxel::SpawnVoxel(this, EVoxelType::Unknown);
}
