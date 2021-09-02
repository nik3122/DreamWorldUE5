// Fill out your copyright notice in the Description page of Project Settings.


#include "Equip/Equip.h"

#include "CharacterInventory.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/DWCharacter.h"
#include "Inventory/Slot/InventoryEquipSlot.h"

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

void AEquip::Initialize(ADWCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	if (OwnerCharacter)
	{
		OwnerCharacter->GetInventory()->GetSplitSlots<UInventoryEquipSlot>(ESplitSlotType::Equip)[(int32)GetEquipData().PartType];
		AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, *UDWHelper::EnumValueToString(TEXT("EEquipPartType"), (int32)GetEquipData().PartType));
	}
}

void AEquip::SetVisible_Implementation(bool bVisible)
{
	GetRootComponent()->SetVisibility(bVisible, true);
}

void AEquip::OnAssemble_Implementation()
{
	
}

void AEquip::OnDischarge_Implementation()
{
	
}

FEquipData AEquip::GetEquipData() const
{
	return UDWHelper::LoadEquipData(EquipID);
}
