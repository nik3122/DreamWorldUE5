// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/DWHumanCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Human/DWHumanCharacterAnim.h"
#include "Equip/EquipWeapon.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/DWAIBlackboard.h"

ADWHumanCharacter::ADWHumanCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleHalfHeight(69);
	GetCapsuleComponent()->SetCapsuleRadius(24);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->AirControl = 0.3f;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -70));

	InventoryData = FInventoryData();
	InventoryData.Items.SetNumZeroed(22);
	InventoryData.SplitInfos.Add(ESplitSlotType::Default, FSplitSlotInfo(0, 10));
	InventoryData.SplitInfos.Add(ESplitSlotType::Shortcut, FSplitSlotInfo(10, 1));
	InventoryData.SplitInfos.Add(ESplitSlotType::Auxiliary, FSplitSlotInfo(11, 1));
	InventoryData.SplitInfos.Add(ESplitSlotType::Equip, FSplitSlotInfo(12, 6));
	InventoryData.SplitInfos.Add(ESplitSlotType::Skill, FSplitSlotInfo(18, 4));

	BehaviorTreeAsset = LoadObject<UBehaviorTree>(nullptr, TEXT("BehaviorTree'/Game/Blueprints/Character/Human/BT_Human_Sample.BT_Human_Sample'"));
	BlackboardAsset = LoadObject<UDWAIBlackboard>(nullptr, TEXT("Blackboard'/Game/Blueprints/Character/Human/BD_Human_Base.BD_Human_Base'"));
}

// Called when the game starts or when spawned
void ADWHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADWHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

}

void ADWHumanCharacter::SetDamaging(bool bInDamaging)
{
	Super::SetDamaging(bInDamaging);

	if(HasWeapon()) GetWeapon()->SetCollisonEnable(bInDamaging);
}
