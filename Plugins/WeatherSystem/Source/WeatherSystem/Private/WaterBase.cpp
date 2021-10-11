// Fill out your copyright notice in the Description page of Project Settings.
// publisher name:Frey
// 2021 of publication
#include "WaterBase.h"

// Sets default values
AWaterBase::AWaterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//根组件
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	AWaterBase::SetRootComponent(RootComponent);
	WaterComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterComp"));
	WaterComp->SetupAttachment(RootComponent);
	WaterMesh = LoadObject<UStaticMesh>(NULL,TEXT("StaticMesh'/WeatherSystem/Water/WaterMesh.WaterMesh'"));
	WaterComp->SetStaticMesh(WaterMesh);
	WaterComp->SetRelativeScale3D(FVector(10.0f));
	WaterComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaterComp->CastShadow = false;
	WaterMaterial = LoadObject<UMaterialInstance>(NULL, TEXT("MaterialInstanceConstant'/WeatherSystem/Water/Materials/M_Water_Frozen.M_Water_Frozen'"));
	WaterComp->SetMaterial(0, WaterMaterial);
}

void AWaterBase::SetWaterBloackAll()
{
    WaterComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WaterComp->SetCollisionResponseToAllChannels(ECR_Block);
	//WH_LOG(LogTemp, Error, TEXT("WeatherSystem_SetWaterBloackAll"));
}

void AWaterBase::SetWaterNoCollision()
{
    WaterComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaterComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	//WH_LOG(LogTemp, Error, TEXT("WeatherSystem_SetWaterNoCollision"));
}

// Called when the game starts or when spawned
void AWaterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWaterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

