// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/DWPlayerCharacterCameraManager.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"

ADWPlayerCharacterCameraManager::ADWPlayerCharacterCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;

	MinCameraDistance = 100.f;
	MaxCameraDistance = 300.f;
	InitCameraDistance = 150.f;
	CameraZoomSpeed = 50.f;

	TargetCameraDistance = 0.f;
}

void ADWPlayerCharacterCameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void ADWPlayerCharacterCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetCameraBoom() && GetCameraBoom()->TargetArmLength != TargetCameraDistance)
	{
		GetCameraBoom()->TargetArmLength = FMath::FInterpTo(GetCameraBoom()->TargetArmLength, TargetCameraDistance, DeltaSeconds, CameraZoomSpeed);
	}
}

float ADWPlayerCharacterCameraManager::GetCameraDistance() const
{
	return TargetCameraDistance;
}

void ADWPlayerCharacterCameraManager::SetCameraDistance(float InCameraDistance, bool bInstant)
{
	TargetCameraDistance = InCameraDistance != -1.f ? FMath::Clamp(InCameraDistance, MinCameraDistance, MaxCameraDistance) : InitCameraDistance;
	if(bInstant && GetCameraBoom()) GetCameraBoom()->TargetArmLength = TargetCameraDistance;
}

void ADWPlayerCharacterCameraManager::ZoomCamera(float InDeltaValue)
{
	if (InDeltaValue != 0.f && GetCameraBoom())
	{
		SetCameraDistance(GetCameraBoom()->TargetArmLength + InDeltaValue);
	}
}

USpringArmComponent* ADWPlayerCharacterCameraManager::GetCameraBoom() const
{
	if (ADWPlayerCharacterController* PlayerController = Cast<ADWPlayerCharacterController>(GetOwningPlayerController()))
	{
		if (ADWPlayerCharacter* PlayerCharacter = Cast<ADWPlayerCharacter>(PlayerController->GetPawn()))
		{
			return PlayerCharacter->GetCameraBoom();
		}
	}
	return nullptr;
}
