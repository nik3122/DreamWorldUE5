// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/DWPlayerCharacterController.h"

#include "CharacterInventory.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "World/WorldManager.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "Engine.h"
#include "Widget/WidgetPrimaryPanel.h"
#include "DWGameInstance.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Inventory.h"
#include "Character/Player/DWPlayerCharacterCameraManager.h"

ADWPlayerCharacterController::ADWPlayerCharacterController()
{
	static ConstructorHelpers::FClassFinder<ADWPlayerCharacterCameraManager> ClassFinder(TEXT("Blueprint'/Game/Blueprints/Character/Player/BP_PlayerCameraManager.BP_PlayerCameraManager_C'"));
	if (ClassFinder.Succeeded())
	{
		PlayerCameraManagerClass = ClassFinder.Class;
	}
}

void ADWPlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

}

void ADWPlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<ADWPlayerCharacter>(InPawn);
	if (PossessedCharacter)
	{
		UDWHelper::GetWidgetInventoryBar()->InitInventory(PossessedCharacter->GetInventory());
		UDWHelper::GetWidgetInventoryPanel()->InitInventory(PossessedCharacter->GetInventory());
		PossessedCharacter->Refresh();
	}
}

void ADWPlayerCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	if (PossessedCharacter)
	{
		UDWHelper::GetWidgetInventoryBar()->InitInventory(nullptr);
		UDWHelper::GetWidgetInventoryPanel()->InitInventory(nullptr);
		PossessedCharacter = nullptr;
	}
}

void ADWPlayerCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADWPlayerCharacterController::RaycastFromAimPoint(FHitResult& OutHitResult, EGameTraceType InGameTraceType, float InRayDistance)
{
	int32 viewportSizeX, viewportSizeY;
	FVector sightPos, rayDirection;
	GetViewportSize(viewportSizeX, viewportSizeY);
	if (DeprojectScreenPositionToWorld(viewportSizeX * 0.5f, viewportSizeY * 0.5f, sightPos, rayDirection))
	{
		FVector rayStart = PlayerCameraManager->GetCameraLocation();
		FVector rayEnd = rayStart + rayDirection * InRayDistance;
		return UKismetSystemLibrary::LineTraceSingle(PossessedCharacter, rayStart, rayEnd, UDWHelper::GetWorldManager()->GetGameTrace(InGameTraceType), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
	}
	return false;
}