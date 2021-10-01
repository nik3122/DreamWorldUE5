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
#include "DWGameState.h"
#include "InventorySlot.h"
#include "PlayerDataSave.h"
#include "WidgetModuleBPLibrary.h"
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
	
	// camera
	CameraTurnRate = 45;
	CameraLookUpRate = 45;
	CameraDistanceRange = FVector2D(100, 300);
}

void ADWPlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if(AWorldManager* WorldManager = AWorldManager::GetCurrent())
	{
		WorldManager->OnBasicGenerated.AddDynamic(this, &ADWPlayerCharacterController::OnBasicGenerated);
	}
}

void ADWPlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->SetTickableWhenPaused(true);

	InputComponent->BindAxis("Turn", this, &ADWPlayerCharacterController::TurnCam);
	InputComponent->BindAxis("LookUp", this, &ADWPlayerCharacterController::LookUpCam);
	
	InputComponent->BindAction("ZoomNear", IE_Pressed, this, &ADWPlayerCharacterController::ZoomNear);
	InputComponent->BindAction("ZoomFar", IE_Pressed, this, &ADWPlayerCharacterController::ZoomFar);

	InputComponent->BindAction("UseInventoryItem", IE_Pressed, this, &ADWPlayerCharacterController::UseInventoryItem);
	InputComponent->BindAction("DiscardInventoryItem", IE_Pressed, this, &ADWPlayerCharacterController::DiscardInventoryItem);
	InputComponent->BindAction("PrevInventorySlot", IE_Pressed, this, &ADWPlayerCharacterController::PrevInventorySlot);
	InputComponent->BindAction("NextInventorySlot", IE_Pressed, this, &ADWPlayerCharacterController::NextInventorySlot);
	InputComponent->BindAction("SelectInventorySlot1", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot1);
	InputComponent->BindAction("SelectInventorySlot2", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot2);
	InputComponent->BindAction("SelectInventorySlot3", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot3);
	InputComponent->BindAction("SelectInventorySlot4", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot4);
	InputComponent->BindAction("SelectInventorySlot5", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot5);
	InputComponent->BindAction("SelectInventorySlot6", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot6);
	InputComponent->BindAction("SelectInventorySlot7", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot7);
	InputComponent->BindAction("SelectInventorySlot8", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot8);
	InputComponent->BindAction("SelectInventorySlot9", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot9);
	InputComponent->BindAction("SelectInventorySlot10", IE_Pressed, this, &ADWPlayerCharacterController::SelectInventorySlot10);
	InputComponent->BindAction("ToggleInventoryPanel", IE_Pressed, this, &ADWPlayerCharacterController::ToggleInventoryPanel);
	InputComponent->BindAction("Pause/ContinueGame", IE_Pressed, this, &ADWPlayerCharacterController::PauseOrContinueGame);
}

void ADWPlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<ADWPlayerCharacter>(GetCharacter());
	if (PossessedCharacter)
	{
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetPrimaryPanel>(this, PossessedCharacter);
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryBar>(this, PossessedCharacter);
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryPanel>(this, PossessedCharacter);
		PossessedCharacter->RefreshData();
	}
}

void ADWPlayerCharacterController::OnUnPossess()
{
	if (PossessedCharacter)
	{
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetPrimaryPanel>(this, nullptr);
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryBar>(this, nullptr);
		UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryPanel>(this, nullptr);
		PossessedCharacter->Destroy();
		PossessedCharacter = nullptr;
	}
	Super::OnUnPossess();
}

void ADWPlayerCharacterController::OnBasicGenerated(FVector InPlayerLocation)
{
	if(PossessedCharacter)
	{
		if(PossessedCharacter->GetActorLocation().IsNearlyZero())
		{
			PossessedCharacter->SetActorLocation(InPlayerLocation);
		}
		PossessedCharacter->Active(true);
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
				PlayerCharacter->Disable(true, true);
				PlayerCharacter->LoadData(PlayerDataSave->GetPlayerData());

				Possess(PlayerCharacter);

				if(AWorldManager* WorldManager = AWorldManager::GetCurrent())
				{
					if(UWorldDataSave* WorldDataSave = WorldManager->GetDataSave())
					{
						if (WorldDataSave->IsExistPlayerRecord(InPlayerName))
						{
							PlayerCharacter->LoadRecordData(WorldDataSave->LoadPlayerRecord(InPlayerName));
						}
						else
						{
							if(WorldManager->GetWorldTimer())
							{
								WorldManager->GetWorldTimer()->SetTimeSeconds(0);
							}
							PlayerCharacter->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
							GetCameraManager()->SetCameraDistance(-1.f, true);
							SetControlRotation(PlayerCharacter->GetActorRotation());
						}
					}
				}
				
				if (!PlayerDataSave->GetPlayerData().InventoryData.bSaved)
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
		DataSave = nullptr;
		if(PossessedCharacter)
		{
			if(AWorldManager* WorldManager = AWorldManager::GetCurrent())
			{
				if(UWorldDataSave* WorldDataSave = WorldManager->GetDataSave())
				{
					FPlayerRecordData PlayerRecordData = PossessedCharacter->ToRecordData();
					if(UWorldTimerComponent* WorldTimer = WorldManager->GetWorldTimer())
					{
						PlayerRecordData.TimeSeconds = WorldTimer->GetTimeSeconds();
					}
					WorldDataSave->SavePlayerRecord(PlayerRecordData);
				}
			}
			GameInstance->UnloadPlayerData(PossessedCharacter->GetName());
		}
   }
	UnPossess();
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
		return UKismetSystemLibrary::LineTraceSingle(PossessedCharacter, rayStart, rayEnd, UDWHelper::GetGameTrace(InGameTraceType), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
	}
	return false;
}

void ADWPlayerCharacterController::TurnCam(float InRate)
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			AddYawInput(InRate * CameraTurnRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void ADWPlayerCharacterController::LookUpCam(float InRate)
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			AddPitchInput(InRate * CameraLookUpRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void ADWPlayerCharacterController::ZoomNear()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			GetCameraManager()->ZoomCamera(-50);
		}
	}
}

void ADWPlayerCharacterController::ZoomFar()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			GetCameraManager()->ZoomCamera(50);
		}
	}
}

void ADWPlayerCharacterController::ToggleInventoryPanel()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::ToggleUserWidget<UWidgetInventoryPanel>(this, false, UDWHelper::LoadWidgetInventoryPanelClass());
		}
	}
}

void ADWPlayerCharacterController::UseInventoryItem()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			const int32 Count = GetPossessedCharacter() && GetPossessedCharacter()->IsPressedSprint() ? -1 : 1;
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedSlot()->UseItem(Count);
		}
	}
}

void ADWPlayerCharacterController::DiscardInventoryItem()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			const int32 Count = GetPossessedCharacter() && GetPossessedCharacter()->IsPressedSprint() ? -1 : 1;
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedSlot()->DiscardItem(Count);
		}
	}
}

void ADWPlayerCharacterController::PrevInventorySlot()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->PrevInventorySlot();
		}
	}
}

void ADWPlayerCharacterController::NextInventorySlot()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->NextInventorySlot();
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot1()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(0);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot2()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(1);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot3()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(2);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot4()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(3);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot5()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(4);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot6()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(5);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot7()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(6);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot8()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(7);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot9()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(8);
		}
	}
}

void ADWPlayerCharacterController::SelectInventorySlot10()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->SelectInventorySlot(9);
		}
	}
}

void ADWPlayerCharacterController::PauseOrContinueGame()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (ADWGameMode* GameMode = UDWHelper::GetGameMode(this))
		{
			if (GameState->GetCurrentState() == EGameState::Playing)
			{
				GameMode->PauseGame();
			}
			else if (GameState->GetCurrentState() == EGameState::Pausing)
			{
				GameMode->UnPauseGame();
			}
		}
	}
}

ADWPlayerCharacterCameraManager* ADWPlayerCharacterController::GetCameraManager() const
{
	return Cast<ADWPlayerCharacterCameraManager>(PlayerCameraManager);
}
