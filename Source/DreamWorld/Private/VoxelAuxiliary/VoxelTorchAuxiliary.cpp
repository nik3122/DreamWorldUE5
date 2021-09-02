// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelTorchAuxiliary.h"
#include "Voxel/Voxel.h"
#include "Components/PointLightComponent.h"
#include "World/WorldManager.h"

// Sets default values
AVoxelTorchAuxiliary::AVoxelTorchAuxiliary()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComponent"));
	LightComponent->SetupAttachment(RootComponent);
	LightComponent->SetLightColor(FLinearColor(1, 0.75f, 0, 1));
	LightComponent->SetIntensity(50000.f);
	LightComponent->SetAttenuationRadius(1500.f);
	LightComponent->SetCastShadows(false);
}

// Called when the game starts or when spawned
void AVoxelTorchAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVoxelTorchAuxiliary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelTorchAuxiliary::Initialize(UVoxel* InOwner, FVector InLocaltion)
{
	Super::Initialize(InOwner, InLocaltion);

	if(!InOwner || !InOwner->IsValidLowLevel()) return;

	LightComponent->SetRelativeLocation(FVector::UpVector * InOwner->GetVoxelData().GetFinalRange().Z * 0.5f * AWorldManager::GetWorldInfo().BlockSize);
}
