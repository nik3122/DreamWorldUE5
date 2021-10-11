
// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelDoorAuxiliary.h"

#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Voxel/VoxelDoor.h"

// Sets default values
AVoxelDoorAuxiliary::AVoxelDoorAuxiliary()
{
	InteractOptions.Add(EInteractOption::Open);
	InteractOptions.Add(EInteractOption::Close);
}

// Called when the game starts or when spawned
void AVoxelDoorAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelDoorAuxiliary::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OpenDoor();
}

void AVoxelDoorAuxiliary::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CloseDoor();
}

void AVoxelDoorAuxiliary::Initialize(AChunk* InOwnerChunk, FIndex InVoxelIndex)
{
	Super::Initialize(InOwnerChunk, InVoxelIndex);
}

bool AVoxelDoorAuxiliary::OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption)
{
	if(!Super::OnInteract(InTrigger, InInteractOption)) return false;

	if(InteractOptions.Contains(InInteractOption))
	{
		switch (InInteractOption)
		{
			case EInteractOption::Open:
			{
				OpenDoor();
				break;
			}
			case EInteractOption::Close:
			{
				CloseDoor();
				break;
			}
			default: break;
		}
		return true;
	}
	return false;
}

void AVoxelDoorAuxiliary::OpenDoor()
{
	if(UVoxel* Voxel = GetVoxelItem().GetVoxel())
	{
		if(UVoxelDoor* VoxelDoor = Cast<UVoxelDoor>(Voxel))
		{
			VoxelDoor->OpenTheDoor();
		}
		UVoxel::DespawnVoxel(Voxel);
	}
}

void AVoxelDoorAuxiliary::CloseDoor()
{
	if(UVoxel* Voxel = GetVoxelItem().GetVoxel())
	{
		if(UVoxelDoor* VoxelDoor = Cast<UVoxelDoor>(Voxel))
		{
			VoxelDoor->CloseTheDoor();
		}
		UVoxel::DespawnVoxel(Voxel);
	}
}
