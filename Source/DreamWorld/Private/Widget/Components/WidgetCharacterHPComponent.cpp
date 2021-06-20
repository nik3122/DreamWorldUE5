// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/WidgetCharacterHPComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Character/DWCharacter.h"
#include "Widget/Worlds/WidgetCharacterHP.h"

UWidgetCharacterHPComponent::UWidgetCharacterHPComponent()
{
	SetWidgetClass(UDWHelper::LoadWidgetCharacterHPClass());
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(220, 60));
	SetPivot(FVector2D(0.5f, 1));
	SetVisibility(false);
}

void UWidgetCharacterHPComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ADWCharacter>(GetOwner());
}

void UWidgetCharacterHPComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerCharacter) return;

	RefreshVisibility();
}

void UWidgetCharacterHPComponent::RefreshVisibility()
{
	SetVisibility(!OwnerCharacter->IsDead() && UDWHelper::GetPlayerCharacter() && FVector::Distance(OwnerCharacter->GetActorLocation(), UDWHelper::GetPlayerCharacter()->GetActorLocation()) < 1000);
}
