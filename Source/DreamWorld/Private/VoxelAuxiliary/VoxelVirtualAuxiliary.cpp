// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelVirtualAuxiliary.h"
#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AVoxelVirtualAuxiliary::AVoxelVirtualAuxiliary()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("DW_VirtualAuxiliary"));
	BoxComponent->SetBoxExtent(FVector(20, 20, 20));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AVoxelVirtualAuxiliary::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AVoxelVirtualAuxiliary::OnEndOverlap);
	SetRootComponent(BoxComponent);
}

// Called when the game starts or when spawned
void AVoxelVirtualAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelVirtualAuxiliary::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AVoxelVirtualAuxiliary::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called every frame
void AVoxelVirtualAuxiliary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelVirtualAuxiliary::Initialize(UVoxel* InOwner, FVector InLocaltion)
{
	Super::Initialize(InOwner, InLocaltion);

	if(!InOwner || !InOwner->IsValidLowLevel()) return;

	BoxComponent->SetBoxExtent(InOwner->GetVoxelData().GetFinalRange() * 0.5f);
}
