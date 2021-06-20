// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SkillMelee.h"
#include "Vitality/Vitality.h"
#include "Character/DWCharacter.h"

// Sets default values
ASkillMelee::ASkillMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DurationTime = 10;
}

// Called when the game starts or when spawned
void ASkillMelee::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASkillMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
