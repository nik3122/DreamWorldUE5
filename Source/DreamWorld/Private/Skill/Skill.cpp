// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Skill.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/DWCharacter.h"
#include "TimerManager.h"

// Sets default values
ASkill::ASkill()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DurationTime = 0;
	SocketName = NAME_None;
	SkillAbilityIndex = NAME_None;
	OwnerCharacter = nullptr;
}

// Called when the game starts or when spawned
void ASkill::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]() { Destroy(); }, DurationTime, false);
}

// Called every frame
void ASkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkill::Initlize(ADWCharacter* InOwnerCharacter, const FName& InSkillAbilityIndex)
{
	if (InOwnerCharacter)
	{
		OwnerCharacter = InOwnerCharacter;
		SkillAbilityIndex = InSkillAbilityIndex;
		SetActorLocation(OwnerCharacter->GetMesh()->GetSocketLocation(SocketName));
		SetActorRotation(OwnerCharacter->GetActorRotation());
	}
}

void ASkill::Destroyed()
{
	Super::Destroyed();
	GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
}

FCharacterSkillAbilityData ASkill::GetSkillData()
{
	return OwnerCharacter->GetSkillAbility(SkillAbilityIndex);
}
