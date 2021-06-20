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

	OwnerVoxel = nullptr;
}

// Called when the game starts or when spawned
void AVoxelAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVoxelAuxiliary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelAuxiliary::Initialize(UVoxel* InOwner, FVector InLocaltion)
{
	if(!InOwner || !InOwner->IsValidLowLevel()) return;

	OwnerVoxel = InOwner;
	SetActorRelativeLocation(InLocaltion);
	SetActorRelativeRotation(InOwner->GetRotation());
}