// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Components/InteractionComponent.h"

#include "DWCharacter.h"
#include "DWPlayerCharacter.h"
#include "GlobalToolsBPLibrary.h"
#include "WidgetModuleBPLibrary.h"
#include "WidgetPrimaryPanel.h"

UInteractionComponent::UInteractionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	
	UPrimitiveComponent::SetCollisionProfileName(TEXT("DW_Interaction"));
	InitBoxExtent(FVector(100, 100, 50));
	OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnEndOverlap);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!GetOwnerInteraction() || OtherActor == GetOwner()) return;
	
	if(IInteraction* OtherInteraction = Cast<IInteraction>(OtherActor))
	{
		if(!OtherInteraction->GetInteractingTarget())
		{
			OtherInteraction->SetInteractingTarget(GetOwnerInteraction());
		}
	}
}

void UInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!GetOwnerInteraction() || OtherActor == GetOwner()) return;
	
	if(IInteraction* OtherInteraction = Cast<IInteraction>(OtherActor))
	{
		if(OtherInteraction->GetInteractingTarget() == GetOwnerInteraction())
		{
			ADWCharacter* OtherCharacter = Cast<ADWCharacter>(OtherInteraction);
			if(!OtherCharacter || !OtherCharacter->IsRiding())
			{
				OtherInteraction->SetInteractingTarget(nullptr);
			}
		}
	}
}

IInteraction* UInteractionComponent::GetOwnerInteraction() const
{
	return Cast<IInteraction>(GetOwner());
}
