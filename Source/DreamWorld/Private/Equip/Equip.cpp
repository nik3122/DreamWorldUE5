// Fill out your copyright notice in the Description page of Project Settings.


#include "Equip/Equip.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/DWCharacter.h"

// Sets default values
AEquip::AEquip()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCastShadow(false);
	RootComponent = MeshComponent;

	EquipID = NAME_None;
}

// Called when the game starts or when spawned
void AEquip::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEquip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquip::Initlize(ADWCharacter* InOwnerCharacter)
{
	if (InOwnerCharacter)
	{
		OwnerCharacter = InOwnerCharacter;
		UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEquipPartType"), true);
		AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, *EnumPtr->GetAuthoredNameStringByValue((int)GetEquipData().PartType));
	}
}

void AEquip::SetVisible_Implementation(bool bVisible)
{
	GetRootComponent()->SetVisibility(bVisible, true);
}

void AEquip::OnAssemble_Implementation()
{
	EffectHandle = OwnerCharacter->ApplyEffect(GetEquipData().EffectClass);
}

void AEquip::OnDischarge_Implementation()
{
	OwnerCharacter->RemoveEffect(EffectHandle);
}

FEquipData AEquip::GetEquipData()
{
	return UDWHelper::LoadEquipData(EquipID);
}
