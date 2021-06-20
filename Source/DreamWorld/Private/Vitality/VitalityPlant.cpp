// Fill out your copyright notice in the Description page of Project Settings.


#include "Vitality/VitalityPlant.h"
#include "Components/BoxComponent.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "Voxel/Voxel.h"

// Sets default values
AVitalityPlant::AVitalityPlant()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AVitalityPlant::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AVitalityPlant::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVitalityPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
