// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/DWMonsterCharacter.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SkeletalMeshComponent.h"
#include "AI/DWAIBlackboard.h"

ADWMonsterCharacter::ADWMonsterCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryData = FInventoryData();
	InventoryData.Items.SetNum(16);
	InventoryData.SplitInfos.Add(ESplitSlotType::Default, FSplitSlotInfo(0, 10));
	InventoryData.SplitInfos.Add(ESplitSlotType::Equip, FSplitSlotInfo(10, 6));

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh(), TEXT("AttackPoint"));
	AttackPoint->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	AttackPoint->SetRelativeScale3D(FVector(1, 1, 1));
	AttackPoint->SetBoxExtent(FVector(20, 20, 20));
	AttackPoint->SetCollisionProfileName(TEXT("DW_Weapon"));
	AttackPoint->SetGenerateOverlapEvents(false);
	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ADWMonsterCharacter::OnAttackPointOverlap);

	BehaviorTreeAsset = LoadObject<UBehaviorTree>(nullptr, TEXT("BehaviorTree'/Game/Blueprints/Character/Monster/BT_Monster_Sample.BT_Monster_Sample'"));
	BlackboardAsset = LoadObject<UDWAIBlackboard>(nullptr, TEXT("Blackboard'/Game/Blueprints/Character/Monster/BD_Monster_Base.BD_Monster_Base'"));
}

// Called when the game starts or when spawned
void ADWMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADWMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

}

void ADWMonsterCharacter::OnAttackPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnHitTarget(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ADWMonsterCharacter::SetDamaging(bool bInDamaging)
{
	Super::SetDamaging(bInDamaging);

	AttackPoint->SetGenerateOverlapEvents(bInDamaging);
}
