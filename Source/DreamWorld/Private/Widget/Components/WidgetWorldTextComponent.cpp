// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/WidgetWorldTextComponent.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Widget/Worlds/WidgetWorldText.h"

UWidgetWorldTextComponent::UWidgetWorldTextComponent()
{
	SetWidgetClass(UDWHelper::LoadWidgetWorldTextClass());
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(200, 50));
	SetPivot(FVector2D(0.5f, 1));
}

void UWidgetWorldTextComponent::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle destroyTimer;
	GetWorld()->GetTimerManager().SetTimer(destroyTimer, [this]() {
		if (this && this->IsValidLowLevel())
		{
			DestroyComponent();
		}
	}, 1, false);
}

void UWidgetWorldTextComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshVisibility();
	AddRelativeLocation(FVector::UpVector * DeltaTime * 100);
}

void UWidgetWorldTextComponent::RefreshVisibility()
{
	ADWPlayerCharacter* PlayerCharacter = UDWHelper::GetPlayerCharacter(this);
	SetVisibility(PlayerCharacter && FVector::Distance(GetOwner()->GetActorLocation(), PlayerCharacter->GetActorLocation()) < 1000);
}
