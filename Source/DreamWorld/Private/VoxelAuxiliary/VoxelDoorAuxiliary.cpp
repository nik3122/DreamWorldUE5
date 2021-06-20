
// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelDoorAuxiliary.h"
#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Voxel/VoxelDoor.h"

// Sets default values
AVoxelDoorAuxiliary::AVoxelDoorAuxiliary()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AVoxelDoorAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelDoorAuxiliary::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Cast<UVoxelDoor>(OwnerVoxel)->OpenTheDoor();
}

void AVoxelDoorAuxiliary::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Cast<UVoxelDoor>(OwnerVoxel)->CloseTheDoor();
}

// Called every frame
void AVoxelDoorAuxiliary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelDoorAuxiliary::Initialize(UVoxel* InOwner, FVector InLocaltion)
{
	Super::Initialize(InOwner, InLocaltion);
}
