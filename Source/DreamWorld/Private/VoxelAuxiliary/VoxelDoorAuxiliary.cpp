
// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelDoorAuxiliary.h"

#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Voxel/VoxelDoor.h"

// Sets default values
AVoxelDoorAuxiliary::AVoxelDoorAuxiliary()
{

}

// Called when the game starts or when spawned
void AVoxelDoorAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelDoorAuxiliary::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(UVoxel* Voxel = GetVoxelItem().GetVoxel())
	{
		if(UVoxelDoor* VoxelDoor = Cast<UVoxelDoor>(Voxel))
		{
			VoxelDoor->OpenTheDoor();
		}
		UVoxel::DespawnVoxel(this, Voxel);
	}
}

void AVoxelDoorAuxiliary::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(UVoxel* Voxel = GetVoxelItem().GetVoxel())
	{
		if(UVoxelDoor* VoxelDoor = Cast<UVoxelDoor>(Voxel))
		{
			VoxelDoor->CloseTheDoor();
		}
		UVoxel::DespawnVoxel(this, Voxel);
	}
}

void AVoxelDoorAuxiliary::Initialize(AChunk* InOwnerChunk, FIndex InVoxelIndex)
{
	Super::Initialize(InOwnerChunk, InVoxelIndex);
}
