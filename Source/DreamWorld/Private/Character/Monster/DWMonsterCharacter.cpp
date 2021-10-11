// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/DWMonsterCharacter.h"

#include "CharacterInventory.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "DWHumanCharacter.h"
#include "InventorySlot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SkeletalMeshComponent.h"
#include "AI/DWAIBlackboard.h"

ADWMonsterCharacter::ADWMonsterCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh(), TEXT("AttackPoint"));
	AttackPoint->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	AttackPoint->SetRelativeScale3D(FVector(1, 1, 1));
	AttackPoint->SetBoxExtent(FVector(20, 20, 20));
	AttackPoint->SetCollisionProfileName(TEXT("DW_Weapon"));
	AttackPoint->SetGenerateOverlapEvents(false);
	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ADWMonsterCharacter::OnAttackPointOverlap);

	InteractOptions.Add(EInteractOption::Ride);
	InteractOptions.Add(EInteractOption::Feed);
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

bool ADWMonsterCharacter::OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption)
{
	if(!Super::OnInteract(InTrigger, InInteractOption)) return false;

	if(GetInteractOptions(InTrigger).Contains(InInteractOption))
	{
		switch (InInteractOption)
		{
			case EInteractOption::Feed:
			{
				if(ADWCharacter* TriggerCharacter = Cast<ADWCharacter>(InTrigger))
				{
					TriggerCharacter->GetInventory()->SetConnectInventory(GetInventory());
					TriggerCharacter->GetInventory()->GetSelectedSlot()->MoveItem(1);
					TriggerCharacter->GetInventory()->SetConnectInventory(nullptr);
				}
				break;
			}
			case EInteractOption::Ride:
			{
				if(ADWCharacter* TriggerCharacter = Cast<ADWCharacter>(InTrigger))
				{
					TriggerCharacter->Ride(this);
				}
				break;
			}
			case EInteractOption::UnRide:
			{
				if(ADWCharacter* TriggerCharacter = Cast<ADWCharacter>(InTrigger))
				{
					TriggerCharacter->UnRide();
				}
				break;
			}
			default: break;
		}
		return true;
	}
	return false;
}

TArray<EInteractOption> ADWMonsterCharacter::GetInteractOptions(IInteraction* InTrigger) const
{
	TArray<EInteractOption> RetValues = Super::GetInteractOptions(InTrigger);
	if(!bDead)
	{
		for(auto Iter : InteractOptions)
		{
			switch(Iter)
			{
				case EInteractOption::Feed:
				{
					if(ADWCharacter* InTriggerCharacter = Cast<ADWCharacter>(InTrigger))
					{
						if(!IsEnemy(InTriggerCharacter) && UDWHelper::LoadPropData(InTriggerCharacter->GetInventory()->GetSelectedItem().ID).PropType == EPropType::Food)
						{
							RetValues.Add(Iter);
						}
					}
					break;
				}
				case EInteractOption::Ride:
				{
					if(ADWCharacter* InTriggerCharacter = Cast<ADWCharacter>(InTrigger))
					{
						if(!IsEnemy(InTriggerCharacter) && InTriggerCharacter->IsA(ADWHumanCharacter::StaticClass()) && InTriggerCharacter->GetRidingTarget() != this)
						{
							RetValues.Add(Iter);
						}
					}
					break;
				}
				case EInteractOption::UnRide:
				{
					if(ADWCharacter* InTriggerCharacter = Cast<ADWCharacter>(InTrigger))
					{
						if(!IsEnemy(InTriggerCharacter) && InTriggerCharacter->IsA(ADWHumanCharacter::StaticClass()) && InTriggerCharacter->GetRidingTarget() == this)
						{
							RetValues.Add(Iter);
						}
					}
					break;
				}
				default: break;
			}
		}
	}
	return RetValues;
}
