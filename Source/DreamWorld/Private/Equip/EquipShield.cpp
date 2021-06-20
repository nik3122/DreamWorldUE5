// Fill out your copyright notice in the Description page of Project Settings.


#include "Equip/EquipShield.h"

// Sets default values
AEquipShield::AEquipShield()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEquipShield::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEquipShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FEquipShieldData AEquipShield::GetShieldData()
{
	return UDWHelper::LoadShieldData(EquipID);
}
