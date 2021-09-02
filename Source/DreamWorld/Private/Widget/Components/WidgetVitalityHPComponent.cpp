// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/WidgetVitalityHPComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Vitality/VitalityObject.h"
#include "Widget/Worlds/WidgetCharacterHP.h"

UWidgetVitalityHPComponent::UWidgetVitalityHPComponent()
{
	SetWidgetClass(UDWHelper::LoadWidgetCharacterHPClass());
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(220, 60));
	SetPivot(FVector2D(0.5f, 1));
	SetVisibility(false);
}

void UWidgetVitalityHPComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerVitality = Cast<AVitalityObject>(GetOwner());
}

void UWidgetVitalityHPComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!OwnerVitality) return;

	RefreshVisibility();
}

void UWidgetVitalityHPComponent::RefreshVisibility()
{
	ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this);
	SetVisibility(!OwnerVitality->IsDead() && PlayerCharacter && FVector::Distance(OwnerVitality->GetActorLocation(), PlayerCharacter->GetActorLocation()) < 1000);
}
