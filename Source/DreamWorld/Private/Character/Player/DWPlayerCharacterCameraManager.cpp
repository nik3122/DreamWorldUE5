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

	CameraBoom = nullptr;
}

void ADWPlayerCharacterCameraManager::BeginPlay()
{
	Super::BeginPlay();

	SetCameraDistance(InitCameraDistance);
}

void ADWPlayerCharacterCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetCameraBoom() && CameraBoom->TargetArmLength != TargetCameraDistance)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetCameraDistance, DeltaSeconds, CameraZoomSpeed);
	}
}

float ADWPlayerCharacterCameraManager::GetCameraDistance()
{
	if(GetCameraBoom())
	{
		return CameraBoom->TargetArmLength;
	}
	return 0.f;
}

void ADWPlayerCharacterCameraManager::SetCameraDistance(float InCameraDistance)
{
	TargetCameraDistance = FMath::Clamp(InCameraDistance, MinCameraDistance, MaxCameraDistance);
}

void ADWPlayerCharacterCameraManager::ZoomCamera(float InDeltaValue)
{
	if (InDeltaValue != 0.f)
	{
		SetCameraDistance(CameraBoom->TargetArmLength + InDeltaValue);
	}
}

USpringArmComponent* ADWPlayerCharacterCameraManager::GetCameraBoom()
{
	if(CameraBoom == nullptr)
	{
		ADWPlayerCharacterController* PlayerController = Cast<ADWPlayerCharacterController>(GetOwningPlayerController());
		if (PlayerController != nullptr)
		{
			ADWPlayerCharacter* PlayerCharacter = Cast<ADWPlayerCharacter>(PlayerController->GetPawn());
			if (PlayerCharacter != nullptr)
			{
				CameraBoom = PlayerCharacter->GetCameraBoom();
			}
		}
	}
	return CameraBoom;
}
