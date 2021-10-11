// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/DWPlayerController.h"

#include "CharacterInventory.h"
#include "DWGameMode.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "World/WorldManager.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "Engine.h"
#include "Widget/WidgetPrimaryPanel.h"
#include "Gameplay/DWGameInstance.h"
#include "Gameplay/DWGameState.h"
#include "InventorySlot.h"
#include "PlayerDataSave.h"
#include "WidgetModuleBPLibrary.h"
#include "WorldDataSave.h"
#include "WorldTimerComponent.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/Inventory.h"
#include "Character/Player/DWPlayerCharacterCameraManager.h"

UPlayerDataSave* ADWPlayerController::DataSave = nullptr;

ADWPlayerController::ADWPlayerController()
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

	// inputs
	bPressedSprint = false;
	DoubleJumpTime = 0.f;
}

void ADWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(AWorldManager* WorldManager = AWorldManager::Get())
	{
		WorldManager->OnBasicGenerated.AddDynamic(this, &ADWPlayerController::OnBasicGenerated);
	}
}

void ADWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->SetTickableWhenPaused(true);

	InputComponent->BindAxis("Turn", this, &ADWPlayerController::TurnCam);
	InputComponent->BindAxis("LookUp", this, &ADWPlayerController::LookUpCam);
	
	InputComponent->BindAction("ZoomNear", IE_Pressed, this, &ADWPlayerController::ZoomNear);
	InputComponent->BindAction("ZoomFar", IE_Pressed, this, &ADWPlayerController::ZoomFar);

	InputComponent->BindAxis("MoveForward", this, &ADWPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADWPlayerController::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ADWPlayerController::MoveUp);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ADWPlayerController::OnJumpPressed);
	InputComponent->BindAction("Jump", IE_Released, this, &ADWPlayerController::OnJumpReleased);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ADWPlayerController::OnSprintPressed);
	InputComponent->BindAction("Sprint", IE_Released, this, &ADWPlayerController::OnSprintReleased);

	InputComponent->BindAction("UseInventoryItem", IE_Pressed, this, &ADWPlayerController::UseInventoryItem);
	InputComponent->BindAction("UseAllInventoryItem", IE_Pressed, this, &ADWPlayerController::UseAllInventoryItem);
	InputComponent->BindAction("DiscardInventoryItem", IE_Pressed, this, &ADWPlayerController::DiscardInventoryItem);
	InputComponent->BindAction("DiscardAllInventoryItem", IE_Pressed, this, &ADWPlayerController::DiscardAllInventoryItem);
	InputComponent->BindAction("PrevInventorySlot", IE_Pressed, this, &ADWPlayerController::PrevInventorySlot);
	InputComponent->BindAction("NextInventorySlot", IE_Pressed, this, &ADWPlayerController::NextInventorySlot);
	InputComponent->BindAction("SelectInventorySlot1", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot1);
	InputComponent->BindAction("SelectInventorySlot2", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot2);
	InputComponent->BindAction("SelectInventorySlot3", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot3);
	InputComponent->BindAction("SelectInventorySlot4", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot4);
	InputComponent->BindAction("SelectInventorySlot5", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot5);
	InputComponent->BindAction("SelectInventorySlot6", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot6);
	InputComponent->BindAction("SelectInventorySlot7", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot7);
	InputComponent->BindAction("SelectInventorySlot8", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot8);
	InputComponent->BindAction("SelectInventorySlot9", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot9);
	InputComponent->BindAction("SelectInventorySlot10", IE_Pressed, this, &ADWPlayerController::SelectInventorySlot10);
	
	InputComponent->BindAction("ToggleInventoryPanel", IE_Pressed, this, &ADWPlayerController::ToggleInventoryPanel);

	InputComponent->BindAction("Pause/ContinueGame", IE_Pressed, this, &ADWPlayerController::PauseOrContinueGame);
}

void ADWPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ADWPlayerCharacter* InCharacter = Cast<ADWPlayerCharacter>(InPawn))
	{
		if(InCharacter != PlayerCharacter)
		{
			PlayerCharacter = InCharacter;
			PlayerCharacter->SetOwnerController(this);
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetPrimaryPanel>(PlayerCharacter);
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryBar>(PlayerCharacter);
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryPanel>(PlayerCharacter);
			PlayerCharacter->RefreshData();
		}
	}
}

void ADWPlayerController::OnUnPossess()
{
	if (ADWPlayerCharacter* OwnerCharacter = Cast<ADWPlayerCharacter>(GetPawn()))
	{
		if(OwnerCharacter == PlayerCharacter && !PlayerCharacter->IsRiding())
		{
			PlayerCharacter->SetOwnerController(nullptr);
			PlayerCharacter = nullptr;
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetPrimaryPanel>(nullptr);
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryBar>(nullptr);
			UWidgetModuleBPLibrary::InitializeUserWidget<UWidgetInventoryPanel>(nullptr);
		}
	}
	Super::OnUnPossess();
}

void ADWPlayerController::OnBasicGenerated(FVector InPlayerLocation)
{
	if(PlayerCharacter)
	{
		if(PlayerCharacter->GetActorLocation().IsNearlyZero())
		{
			PlayerCharacter->SetActorLocation(InPlayerLocation);
		}
		PlayerCharacter->Active(true);
	}
}

void ADWPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetProcessedCharacter() || GetProcessedCharacter()->IsDead()) return;

	if (GetProcessedCharacter()->IsActive())
	{
		if (bPressedSprint && GetProcessedCharacter()->GetMoveVelocity().Size() > 0.2f)
		{
			GetProcessedCharacter()->Sprint();
		}
		else
		{
			GetProcessedCharacter()->UnSprint();
		}

		if (DoubleJumpTime > 0.f)
		{
			DoubleJumpTime -= DeltaTime;
		}
	}
}

void ADWPlayerController::LoadPlayer(const FString& InPlayerName)
{
	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
	{
		if(UPlayerDataSave* PlayerDataSave = GameInstance->LoadPlayerData(InPlayerName))
		{
			DataSave = PlayerDataSave;

			FActorSpawnParameters spawnParams = FActorSpawnParameters();
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (ADWPlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<ADWPlayerCharacter>(PlayerDataSave->GetPlayerData().GetCharacterData().Class, spawnParams))
			{
				NewPlayerCharacter->Disable(true, true);
				NewPlayerCharacter->LoadData(PlayerDataSave->GetPlayerData());

				Possess(NewPlayerCharacter);

				if(AWorldManager* WorldManager = AWorldManager::Get())
				{
					if(UWorldDataSave* WorldDataSave = WorldManager->GetDataSave())
					{
						if (WorldDataSave->IsExistPlayerRecord(InPlayerName))
						{
							NewPlayerCharacter->LoadRecordData(WorldDataSave->LoadPlayerRecord(InPlayerName));
						}
						else
						{
							if(WorldManager->GetWorldTimer())
							{
								WorldManager->GetWorldTimer()->SetTimeSeconds(0);
							}
							NewPlayerCharacter->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
							GetCameraManager()->SetCameraDistance(-1.f, true);
							SetControlRotation(NewPlayerCharacter->GetActorRotation());
						}
					}
				}
				
				if (!PlayerDataSave->GetPlayerData().InventoryData.bSaved)
				{
					auto VoxelDatas = UDWHelper::LoadVoxelDatas();
					for (int32 i = 0; i < VoxelDatas.Num(); i++)
					{
						FItem tmpItem = FItem(VoxelDatas[i].ID, VoxelDatas[i].MaxCount);
						NewPlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto EquipDatas = UDWHelper::LoadEquipDatas();
					for (int32 i = 0; i < EquipDatas.Num(); i++)
					{
						FItem tmpItem = FItem(EquipDatas[i].ID, EquipDatas[i].MaxCount);
						NewPlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto PropDatas = UDWHelper::LoadPropDatas();
					for (int32 i = 0; i < PropDatas.Num(); i++)
					{
						FItem tmpItem = FItem(PropDatas[i].ID, PropDatas[i].MaxCount);
						NewPlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}

					auto SkillDatas = UDWHelper::LoadSkillDatas();
					for (int32 i = 0; i < SkillDatas.Num(); i++)
					{
						FItem tmpItem = FItem(SkillDatas[i].ID, SkillDatas[i].MaxCount);
						NewPlayerCharacter->GetInventory()->AdditionItems(tmpItem);
					}
				}

				OnPlayerSpawned.Broadcast(NewPlayerCharacter);
			}
		}
	}
}

void ADWPlayerController::UnLoadPlayer()
{
	if(UDWGameInstance* GameInstance = UDWHelper::GetGameInstance(this))
    {
		DataSave = nullptr;
		if(PlayerCharacter)
		{
			if(AWorldManager* WorldManager = AWorldManager::Get())
			{
				if(UWorldDataSave* WorldDataSave = WorldManager->GetDataSave())
				{
					FPlayerRecordSaveData PlayerRecordData = PlayerCharacter->ToRecordData();
					if(UWorldTimerComponent* WorldTimer = WorldManager->GetWorldTimer())
					{
						PlayerRecordData.TimeSeconds = WorldTimer->GetTimeSeconds();
					}
					WorldDataSave->SavePlayerRecord(PlayerRecordData);
				}
			}
			GameInstance->UnloadPlayerData(PlayerCharacter->GetNameC());
		}
   }
	UnPossess();
}

void ADWPlayerController::ResetData()
{
	bPressedSprint = false;
	DoubleJumpTime = 0.f;
}

bool ADWPlayerController::RaycastFromAimPoint(FHitResult& OutHitResult, EGameTraceType InGameTraceType, float InRayDistance) const
{
	int32 viewportSizeX, viewportSizeY;
	FVector sightPos, rayDirection;
	GetViewportSize(viewportSizeX, viewportSizeY);
	if (DeprojectScreenPositionToWorld(viewportSizeX * 0.5f, viewportSizeY * 0.5f, sightPos, rayDirection))
	{
		const FVector rayStart = PlayerCameraManager->GetCameraLocation();
		const FVector rayEnd = rayStart + rayDirection * InRayDistance;
		return UKismetSystemLibrary::LineTraceSingle(PlayerCharacter, rayStart, rayEnd, UDWHelper::GetGameTrace(InGameTraceType), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
	}
	return false;
}

void ADWPlayerController::TurnCam(float InRate)
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			AddYawInput(InRate * CameraTurnRate * GetWorld()->GetDeltaSeconds());
			if(GetPlayerCharacter())
			{
				GetPlayerCharacter()->GetTargetSystem()->TargetActorWithAxisInput(InRate);
			}
		}
	}
}

void ADWPlayerController::LookUpCam(float InRate)
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			AddPitchInput(InRate * CameraLookUpRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void ADWPlayerController::ZoomNear()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			GetCameraManager()->ZoomCamera(-50);
		}
	}
}

void ADWPlayerController::ZoomFar()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			GetCameraManager()->ZoomCamera(50);
		}
	}
}

void ADWPlayerController::MoveForward(float InValue)
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;

	FVector Direction;
	const FRotator Rotation = GetControlRotation();
	if (GetProcessedCharacter()->IsFlying() || GetProcessedCharacter()->IsSwimming())
	{
		Direction = Rotation.Vector();
	}
	else
	{
		const FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	}
	GetProcessedCharacter()->AddMovementInput(Direction, InValue);
}

void ADWPlayerController::MoveRight(float InValue)
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetProcessedCharacter()->AddMovementInput(Direction, InValue);
}

void ADWPlayerController::MoveUp(float InValue)
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	if (GetProcessedCharacter()->IsFlying() || GetProcessedCharacter()->IsSwimming())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		GetProcessedCharacter()->AddMovementInput(FVector(Direction.X * 0.1f, Direction.Y * 0.1f, 1.f) * InValue, 0.5f);
	}
}

void ADWPlayerController::OnJumpPressed()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;

	GetProcessedCharacter()->Jump();
	if(DoubleJumpTime <= 0.f)
	{
		DoubleJumpTime = 0.2f;
	}
	else
	{
		if (!GetProcessedCharacter()->IsFlying())
		{
			GetProcessedCharacter()->Fly();
		}
		else
		{
			GetProcessedCharacter()->UnFly();
		}
		DoubleJumpTime = 0.f;
	}
}

void ADWPlayerController::OnJumpReleased()
{
	GetProcessedCharacter()->UnJump();
}

void ADWPlayerController::OnSprintPressed()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;

	bPressedSprint = true;
}

void ADWPlayerController::OnSprintReleased()
{
	bPressedSprint = false;
}

void ADWPlayerController::ToggleInventoryPanel()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::ToggleUserWidget<UWidgetInventoryPanel>(false, UDWHelper::LoadWidgetInventoryPanelClass());
		}
	}
}

void ADWPlayerController::UseInventoryItem()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->GetSelectedSlot()->UseItem(1);
		}
	}
}

void ADWPlayerController::UseAllInventoryItem()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->GetSelectedSlot()->UseItem(-1);
		}
	}
}

void ADWPlayerController::DiscardInventoryItem()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->GetSelectedSlot()->DiscardItem(1);
		}
	}
}

void ADWPlayerController::DiscardAllInventoryItem()
{
	if(!GetProcessedCharacter() || GetProcessedCharacter()->IsBreakAllInput()) return;
	
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->GetSelectedSlot()->DiscardItem(-1);
		}
	}
}

void ADWPlayerController::PrevInventorySlot()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->PrevInventorySlot();
		}
	}
}

void ADWPlayerController::NextInventorySlot()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->NextInventorySlot();
		}
	}
}

void ADWPlayerController::SelectInventorySlot1()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(0);
		}
	}
}

void ADWPlayerController::SelectInventorySlot2()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(1);
		}
	}
}

void ADWPlayerController::SelectInventorySlot3()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(2);
		}
	}
}

void ADWPlayerController::SelectInventorySlot4()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(3);
		}
	}
}

void ADWPlayerController::SelectInventorySlot5()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(4);
		}
	}
}

void ADWPlayerController::SelectInventorySlot6()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(5);
		}
	}
}

void ADWPlayerController::SelectInventorySlot7()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(6);
		}
	}
}

void ADWPlayerController::SelectInventorySlot8()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(7);
		}
	}
}

void ADWPlayerController::SelectInventorySlot9()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(8);
		}
	}
}

void ADWPlayerController::SelectInventorySlot10()
{
	if (ADWGameState* GameState = UDWHelper::GetGameState(this))
	{
		if (GameState->GetCurrentState() == EGameState::Playing)
		{
			UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->SelectInventorySlot(9);
		}
	}
}

void ADWPlayerController::PauseOrContinueGame()
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

ADWCharacter* ADWPlayerController::GetProcessedCharacter() const
{
	return Cast<ADWCharacter>(GetPawn());
}

ADWPlayerCharacterCameraManager* ADWPlayerController::GetCameraManager() const
{
	return Cast<ADWPlayerCharacterCameraManager>(PlayerCameraManager.Get());
}