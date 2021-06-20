// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelAuxiliary/VoxelEntityAuxiliary.h"
#include "Voxel/Voxel.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AVoxelEntityAuxiliary::AVoxelEntityAuxiliary()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("DW_EntityAuxiliary"));
	BoxComponent->SetBoxExtent(FVector(20, 20, 20));
	BoxComponent->OnComponentHit.AddDynamic(this, &AVoxelEntityAuxiliary::OnCollision);
	SetRootComponent(BoxComponent);
}

// Called when the game starts or when spawned
void AVoxelEntityAuxiliary::BeginPlay()
{
	Super::BeginPlay();

}

void AVoxelEntityAuxiliary::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

// Called every frame
void AVoxelEntityAuxiliary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelEntityAuxiliary::Initialize(UVoxel* InOwner, FVector InLocaltion)
{
	Super::Initialize(InOwner, InLocaltion);

	if(!InOwner || !InOwner->IsValidLowLevel()) return;

	BoxComponent->SetBoxExtent(InOwner->GetVoxelData().GetFinalRange() * 0.5f);
}
