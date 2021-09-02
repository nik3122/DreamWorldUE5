// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SkillRemote.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Vitality/Vitality.h"
#include "Character/DWCharacter.h"
#include "TimerManager.h"

// Sets default values
ASkillRemote::ASkillRemote()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionProfileName(TEXT("DW_Weapon"));
	SphereComponent->SetSphereRadius(50);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkillRemote::OnBeginOverlap);
	SetRootComponent(SphereComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCastShadow(false);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0;
	MovementComponent->BounceVelocityStopSimulatingThreshold = false;

	DurationTime = 10;
	InitialVelocity = FVector(3000, 0, 0);
}

// Called when the game starts or when spawned
void ASkillRemote::BeginPlay()
{
	Super::BeginPlay();

}

void ASkillRemote::Initialize(ADWCharacter* InOwnerCharacter, const FName& InSkillIndex)
{
	Super::Initialize(InOwnerCharacter, InSkillIndex);

	MovementComponent->Velocity = GetActorRotation().RotateVector(InitialVelocity);
}

void ASkillRemote::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnHitTarget(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

// Called every frame
void ASkillRemote::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
