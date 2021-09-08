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
#include "PlayerDataSave.h"
#include "WorldDataSave.h"
#include "WorldTimerComponent.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Inventory.h"
#include "Character/Player/DWPlayerCharacterCameraManager.h"

UPlayerDataSave* ADWPlayerCharacterController::DataSave = nullptr;

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

	if(AWorldManager* WorldManager = AWorldManager::Get())
	{
		WorldManager->OnBasicGenerated.AddDynamic(this, &ADWPlayerCharacterController::OnBasicGenerated);
	}
}

void ADWPlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (GetPlayerCharacter())
	{
		UDWHelper::GetWidgetPanelByClass<UWidgetInventoryBar>(this)->InitInventory(GetPlayerCharacter()->GetInventory());
		UDWHelper::GetWidgetPanelByClass<UWidgetInventoryPanel>(this)->InitInventory(GetPlayerCharacter()->GetInventory());
		GetPlayerCharacter()->Refresh();
	}
}

void ADWPlayerCharacterController::OnUnPossess()
{
	if (GetPlayerCharacter())
	{
		UDWHelper::GetWidgetPanelByClass<UWidgetInventoryBar>(this)->InitInventory(nullptr);
		UDWHelper::GetWidgetPanelByClass<UWidgetInventoryPanel>(this)->InitInventory(nullptr);
		GetPlayerCharacter()->Destroy();
	}
	Super::OnUnPossess();
}

void ADWPlayerCharacterController::OnBasicGenerated(FVector InPlayerLocation)
{
	if(GetPlayerCharacter())
	{
		GetPlayerCharacter()->Active(true);
	}
}

void ADWPlayerCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADWPlayerCharacterController::LoadPlayer(const FString& InPlayerName)
{
	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
	{
		if(UPlayerDataSave* PlayerDataSave = GameInstance->LoadPlayerData(InPlayerName))
		{
			DataSave = PlayerDataSave;
			FActorSpawnParameters spawnParams = FActorSpawnParameters();
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			if (ADWPlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<ADWPlayerCharacter>(PlayerDataSave->GetPlayerData().SpawnClass, spawnParams))
			{
				Possess(PlayerCharacter);
				PlayerCharacter->Disable(true, true);
				PlayerCharacter->LoadData(PlayerDataSave->GetPlayerData());

				if(AWorldManager* WorldManager = AWorldManager::Get())
				{
					if(UWorldDataSave* WorldDataSave = WorldManager->GetDataSave())
					{
						if (WorldDataSave->IsExistPlayerRecord(InPlayerName))
						{
							auto PlayerRecord = WorldDataSave->LoadPlayerRecord(InPlayerName);
							if(WorldManager->GetWorldTimer())
							{
								WorldManager->GetWorldTimer()->SetTimeSeconds(PlayerRecord.TimeSeconds);
							}
							PlayerCharacter->SetActorLocationAndRotation(PlayerRecord.Location, PlayerRecord.Rotation);
						}
						else
						{
							if(WorldManager->GetWorldTimer())
							{
								WorldManager->GetWorldTimer()->SetTimeSeconds(0);
							}
							PlayerCharacter->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
						}
					}
				}
				
				if (!PlayerDataSave->GetPlayerData().InventoryData.bInitialized)
				{
					auto VoxelDatas = UDWHelper::LoadVoxelDatas();
					for (int32 i = 0; i < VoxelDatas.Num(); i++)
					{
						FItem tmpItem = FItem(VoxelDatas[i].ID, VoxelDatas[i].MaxCount);
						PlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto EquipDatas = UDWHelper::LoadEquipDatas();
					for (int32 i = 0; i < EquipDatas.Num(); i++)
					{
						FItem tmpItem = FItem(EquipDatas[i].ID, EquipDatas[i].MaxCount);
						PlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto PropDatas = UDWHelper::LoadPropDatas();
					for (int32 i = 0; i < PropDatas.Num(); i++)
					{
						FItem tmpItem = FItem(PropDatas[i].ID, PropDatas[i].MaxCount);
						PlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto SkillDatas = UDWHelper::LoadSkillDatas();
					for (int32 i = 0; i < SkillDatas.Num(); i++)
					{
						FItem tmpItem = FItem(SkillDatas[i].ID, SkillDatas[i].MaxCount);
						PlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}
				}

				OnPlayerSpawned.Broadcast(PlayerCharacter);
			}
		}
	}
}

void ADWPlayerCharacterController::UnLoadPlayer()
{
	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
    {
		if(GetPlayerCharacter())
		{
			GameInstance->UnloadPlayerData(GetPlayerCharacter()->GetName());
		}
    }
	UnPossess();
}

ADWPlayerCharacter* ADWPlayerCharacterController::GetPlayerCharacter() const
{
	return Cast<ADWPlayerCharacter>(GetCharacter());
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
		return UKismetSystemLibrary::LineTraceSingle(GetPlayerCharacter(), rayStart, rayEnd, UDWHelper::GetGameTrace(InGameTraceType), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
	}
	return false;
}
