// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterAnim.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "World/WorldManager.h"
#include "PickUp/PickUpVoxel.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "DWGameMode.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widget/Components/WidgetCharacterHPComponent.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "DWGameInstance.h"
#include "DataSaves/PlayerDataSave.h"
#include "Inventory/Character/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "DWGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/Character/PlayerInventory.h"
#include "Equip/EquipWeapon.h"
#include "Equip/EquipShield.h"
#include "Inventory/Slot/InventorySkillSlot.h"
#include "Character/Player/DWPlayerCharacterCameraManager.h"
#include "Widget/WidgetPrimaryPanel.h"
#include "Inventory/Slot/InventoryEquipSlot.h"

//////////////////////////////////////////////////////////////////////////
// ADWPlayerCharacter

ADWPlayerCharacter::ADWPlayerCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleHalfHeight(69);
	GetCapsuleComponent()->SetCapsuleRadius(24);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DW_Character"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->AirControl = 0.3f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0, 30, 40);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetMesh()->SetRelativeLocation(FVector(0, 0, -70));

	VoxelMesh = CreateDefaultSubobject<UVoxelMeshComponent>(TEXT("VoxelMesh"));
	VoxelMesh->SetupAttachment(GetMesh(), TEXT("VoxelMesh"));
	VoxelMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	VoxelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VoxelMesh->SetCastShadow(false);
	VoxelMesh->SetVisibility(false);
	VoxelMesh->Initialize(EVoxelMeshType::PickUp);

	HammerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HammerMesh"));
	HammerMesh->SetupAttachment(GetMesh(), TEXT("HammerMesh"));
	HammerMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	HammerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HammerMesh->SetCastShadow(false);
	HammerMesh->SetVisibility(false);

	PreviewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	PreviewCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	PreviewCapture->OrthoWidth = 100;
	PreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	PreviewCapture->SetupAttachment(RootComponent);
	PreviewCapture->SetRelativeLocationAndRotation(FVector(100, 0, 0), FRotator(0, 180, 0));
	
	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MiniMapCapture->OrthoWidth = 1000;
	//MiniMapCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	MiniMapCapture->SetupAttachment(RootComponent);
	MiniMapCapture->SetRelativeLocationAndRotation(FVector(0, 0, 500), FRotator(0, 90, 0));

	Inventory = nullptr;
	
	InventoryData = FInventoryData();
	InventoryData.Items.SetNumZeroed(70);
	InventoryData.SplitInfos.Add(ESplitSlotType::Default, FSplitSlotInfo(0, 40));
	InventoryData.SplitInfos.Add(ESplitSlotType::Shortcut, FSplitSlotInfo(40, 10));
	InventoryData.SplitInfos.Add(ESplitSlotType::Auxiliary, FSplitSlotInfo(50, 1));
	InventoryData.SplitInfos.Add(ESplitSlotType::Generate, FSplitSlotInfo(51, 9));
	InventoryData.SplitInfos.Add(ESplitSlotType::Equip, FSplitSlotInfo(60, 6));
	InventoryData.SplitInfos.Add(ESplitSlotType::Skill, FSplitSlotInfo(66, 4));

	// camera
	CameraTurnRate = 45;
	CameraLookUpRate = 45;
	CameraDistanceRange = FVector2D(100, 300);

	// states
	ControlMode = EControlMode::Fighting;
	Nature = ECharacterNature::Player;

	// stats
	InteractDistance = 500;
	PreviewVoxel = nullptr;

	// inputs
	bPressedAttack = false;
	bPressedDefend = false;
	bPressedSprint = false;

	AttackAbilityQueue = 0;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::Disabled;
}

void ADWPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->SetTickableWhenPaused(true);

	PlayerInputComponent->BindAxis("Turn", this, &ADWPlayerCharacter::TurnCam);
	PlayerInputComponent->BindAxis("LookUp", this, &ADWPlayerCharacter::LookUpCam);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADWPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADWPlayerCharacter::MoveRight);
	
	PlayerInputComponent->BindAction("ZoomNear", IE_Pressed, this, &ADWPlayerCharacter::ZoomNear);
	PlayerInputComponent->BindAction("ZoomFar", IE_Pressed, this, &ADWPlayerCharacter::ZoomFar);

	PlayerInputComponent->BindAction("Revival", IE_Pressed, this, &ADWPlayerCharacter::Revive);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADWPlayerCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADWPlayerCharacter::OnJumpReleased);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ADWPlayerCharacter::OnDodgePressed);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &ADWPlayerCharacter::OnDodgeReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADWPlayerCharacter::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADWPlayerCharacter::OnSprintReleased);

	PlayerInputComponent->BindAction("ToggleFly", IE_Released, this, &ADWPlayerCharacter::ToggleFly);
	PlayerInputComponent->BindAction("ToggleCrouch", IE_Released, this, &ADWPlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("ToggleAutoJump", IE_Pressed, this, &ADWPlayerCharacter::ToggleAutoJump);
	PlayerInputComponent->BindAction("ToggleControlMode", IE_Pressed, this, &ADWPlayerCharacter::ToggleControlMode);
	PlayerInputComponent->BindAction("ToggleLockSightTarget", IE_Pressed, this, &ADWPlayerCharacter::ToggleLockSightTarget);

	PlayerInputComponent->BindAction("Attack/Destroy", IE_Pressed, this, &ADWPlayerCharacter::OnAttackDestroyPressed);
	PlayerInputComponent->BindAction("Attack/Destroy", IE_Released, this, &ADWPlayerCharacter::OnAttackDestroyReleased);

	PlayerInputComponent->BindAction("Defend/Generate", IE_Pressed, this, &ADWPlayerCharacter::OnDefendGeneratePressed);
	PlayerInputComponent->BindAction("Defend/Generate", IE_Released, this, &ADWPlayerCharacter::OnDefendGenerateReleased);

	PlayerInputComponent->BindAction("ReleaseSkillAbility1", IE_Pressed, this, &ADWPlayerCharacter::ReleaseSkillAbility1);
	PlayerInputComponent->BindAction("ReleaseSkillAbility2", IE_Pressed, this, &ADWPlayerCharacter::ReleaseSkillAbility2);
	PlayerInputComponent->BindAction("ReleaseSkillAbility3", IE_Pressed, this, &ADWPlayerCharacter::ReleaseSkillAbility3);
	PlayerInputComponent->BindAction("ReleaseSkillAbility4", IE_Pressed, this, &ADWPlayerCharacter::ReleaseSkillAbility4);

	PlayerInputComponent->BindAction("UseInventoryItem", IE_Pressed, this, &ADWPlayerCharacter::UseInventoryItem);
	PlayerInputComponent->BindAction("DiscardInventoryItem", IE_Pressed, this, &ADWPlayerCharacter::DiscardInventoryItem);
	PlayerInputComponent->BindAction("PrevInventorySlot", IE_Pressed, this, &ADWPlayerCharacter::PrevInventorySlot);
	PlayerInputComponent->BindAction("NextInventorySlot", IE_Pressed, this, &ADWPlayerCharacter::NextInventorySlot);
	PlayerInputComponent->BindAction("SelectInventorySlot1", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot1);
	PlayerInputComponent->BindAction("SelectInventorySlot2", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot2);
	PlayerInputComponent->BindAction("SelectInventorySlot3", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot3);
	PlayerInputComponent->BindAction("SelectInventorySlot4", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot4);
	PlayerInputComponent->BindAction("SelectInventorySlot5", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot5);
	PlayerInputComponent->BindAction("SelectInventorySlot6", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot6);
	PlayerInputComponent->BindAction("SelectInventorySlot7", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot7);
	PlayerInputComponent->BindAction("SelectInventorySlot8", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot8);
	PlayerInputComponent->BindAction("SelectInventorySlot9", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot9);
	PlayerInputComponent->BindAction("SelectInventorySlot10", IE_Pressed, this, &ADWPlayerCharacter::SelectInventorySlot10);
	PlayerInputComponent->BindAction("ToggleInventoryPanel", IE_Pressed, this, &ADWPlayerCharacter::ToggleInventoryPanel);
	PlayerInputComponent->BindAction("Pause/ContinueGame", IE_Pressed, this, &ADWPlayerCharacter::PauseOrContinueGame);
}

// Called when the game starts or when spawned
void ADWPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	WidgetCharacterHP->SetWidget(nullptr);

	PreviewCapture->ShowOnlyActors.Add(this);

	//UDWHelper::GetWorldManager()->CreateTeam(this);

	Disable(true, true);

	SetControlMode(ControlMode);
}

// Called every frame
void ADWPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

	if (bActive)
	{
		if (bPressedSprint)
		{
			Sprint();
		}
		else
		{
			UnSprint();
		}

		if (bPressedAttack)
		{
			OnAttackDestroyRepeat();
		}

		if (bPressedDefend)
		{
			OnDefendGenerateRepeat();
		}

		switch (ControlMode)
		{
			case EControlMode::Fighting:
			{
				if (bPressedAttack || AttackAbilityQueue > 0)
				{
					Attack();
				}
				else if (bFreeToAnimate && AttackType == EAttackType::NormalAttack)
				{
					UnAttack();
				}

				if (bPressedDefend)
				{
					Defend();
				}
				else
				{
					UnDefend();
				}
			}
			case EControlMode::Creating:
			{
				FVoxelHitResult voxelHitResult;
				if (RaycastVoxel(voxelHitResult))
				{
					voxelHitResult.Voxel->OnMouseHover(voxelHitResult);
				}
				break;
			}
		}
	}
}

void ADWPlayerCharacter::LoadData(FCharacterData InSaveData)
{
	if (!Inventory) Inventory = NewObject<UPlayerInventory>(this);

	Super::LoadData(InSaveData);

	if (!InSaveData.bInitialized)
	{
		Name = InSaveData.Name;
	}
}

FCharacterData ADWPlayerCharacter::ToData()
{
	return Super::ToData();
}

void ADWPlayerCharacter::UnAttack()
{
	Super::UnAttack();
	AttackAbilityQueue = 0;
}

void ADWPlayerCharacter::TurnCam(float InRate)
{
	AddControllerYawInput(InRate * CameraTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADWPlayerCharacter::LookUpCam(float InRate)
{
	AddControllerPitchInput(InRate * CameraLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADWPlayerCharacter::MoveForward(float InValue)
{
	if (GetPlayerController() && !bBreakAllInput)
	{
		const FRotator Rotation = GetPlayerController()->GetControlRotation();
		FVector Direction = FVector();
		if (bFlying || bSwimming)
		{
			Direction = Rotation.Vector();
		}
		else
		{
			const FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}
		AddMovementInput(Direction, InValue);
	}
}

void ADWPlayerCharacter::MoveRight(float InValue)
{
	if (GetPlayerController() && !bBreakAllInput)
	{
		const FRotator Rotation = GetPlayerController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, InValue);
	}
}

void ADWPlayerCharacter::ZoomNear()
{
	if (GetCameraManager() != nullptr)
	{
		GetCameraManager()->ZoomCamera(-50);
	}
}

void ADWPlayerCharacter::ZoomFar()
{
	if (GetCameraManager() != nullptr)
	{
		GetCameraManager()->ZoomCamera(50);
	}
}

void ADWPlayerCharacter::Active(bool bResetStats)
{
	Super::Active(bResetStats);
	
	if (bResetStats)
	{
		ControlMode = EControlMode::Fighting;
		bPressedAttack = false;
		bPressedDefend = false;
		bPressedSprint = false;
		LockedTarget = nullptr;
	}
}

void ADWPlayerCharacter::Disable(bool bDisableMovement, bool bDisableCollision)
{
	Super::Disable(bDisableMovement, bDisableCollision);
}

void ADWPlayerCharacter::Revive()
{
	Super::Revive();
	SetControlMode(ControlMode);
}

void ADWPlayerCharacter::Interrupt(float InDuration /* = -1 */, bool bInPlayAnim /* = false */)
{
	Super::Interrupt(InDuration, bInPlayAnim);
	AttackAbilityQueue = 0;
}

void ADWPlayerCharacter::LookAtTarget(ADWCharacter* InTargetCharacter)
{
	Super::LookAtTarget(InTargetCharacter);
	FVector tmpDirection = InTargetCharacter->GetActorLocation() + FVector::DownVector * InTargetCharacter->GetHalfHeight() - GetActorLocation();
	FRotator tmpRotator = tmpDirection.ToOrientationRotator();
	GetPlayerController()->SetControlRotation(tmpRotator);
}

FString ADWPlayerCharacter::GetHeadInfo() const
{
	return FString::Printf(TEXT("Lv.%d \"%s\" (Exp: %d/%d)"), Level, *Name, EXP, GetMaxEXP());
}

void ADWPlayerCharacter::SetControlMode(EControlMode InControlMode)
{
	switch (ControlMode = InControlMode)
	{
		case EControlMode::Fighting:
		{
			VoxelMesh->SetVisibility(false);
			HammerMesh->SetVisibility(false);
			if (HasWeapon()) GetWeapon()->SetVisible(true);
			if (HasShield()) GetShield()->SetVisible(true);
			break;
		}
		case EControlMode::Creating:
		{
			VoxelMesh->SetVisibility(true);
			HammerMesh->SetVisibility(true);
			if (HasWeapon()) GetWeapon()->SetVisible(false);
			if (HasShield()) GetShield()->SetVisible(false);
			break;
		}
	}
}

void ADWPlayerCharacter::AttackStart()
{
	Super::AttackStart();

	switch (AttackType)
	{
		case EAttackType::NormalAttack:
		case EAttackType::FallingAttack:
			if (AttackAbilityQueue > 0)
			{
				AttackAbilityQueue--;
			}
			break;
	}
}

void ADWPlayerCharacter::RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot)
{
	if (!EquipSlot->IsEmpty())
	{
		Super::RefreshEquip(InPartType, EquipSlot);
		if (Equips[InPartType])
		{
			PreviewCapture->ShowOnlyActors.Add(Equips[InPartType]);
		}
	}
	else
	{
		if (Equips[InPartType])
		{
			PreviewCapture->ShowOnlyActors.Remove(Equips[InPartType]);
		}
		Super::RefreshEquip(InPartType, EquipSlot);
	}
}

void ADWPlayerCharacter::UpdateVoxelMesh()
{
	auto tmpItem = UDWHelper::GetWidgetInventoryBar()->GetSelectedItem();
	if (tmpItem.IsValid() && tmpItem.GetData().Type == EItemType::Voxel)
	{
		PreviewVoxel = UVoxel::NewVoxel(tmpItem.ID, this);
		VoxelMesh->BuildVoxel(PreviewVoxel);
		VoxelMesh->CreateMesh(0, false);
	}
	else
	{
		VoxelMesh->ClearMesh();
		if (PreviewVoxel)
		{
			PreviewVoxel->OnDestroy();
			PreviewVoxel = nullptr;
		}
	}
}

void ADWPlayerCharacter::HandleNameChanged(const FString& NewValue)
{
	Super::HandleNameChanged(NewValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHeadInfo(GetHeadInfo());
	UDWHelper::GetWidgetInventoryPanel()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleTeamIDChanged(const FString& NewValue)
{
	Super::HandleTeamIDChanged(NewValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHeadInfo(GetHeadInfo());
	UDWHelper::GetWidgetInventoryPanel()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleRaceIDChanged(const FString& NewValue)
{
	Super::HandleRaceIDChanged(NewValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHeadInfo(GetHeadInfo());
	UDWHelper::GetWidgetInventoryPanel()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleLevelChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleLevelChanged(NewValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHeadInfo(GetHeadInfo());
	UDWHelper::GetWidgetInventoryPanel()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleEXPChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleEXPChanged(NewValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHeadInfo(GetHeadInfo());
	UDWHelper::GetWidgetInventoryPanel()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleHealthChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHealthPercent(NewValue, GetMaxHealth());
	UDWHelper::GetWidgetInventoryPanel()->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleMaxHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxHealthChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetHealthPercent(NewValue, GetMaxHealth());
	UDWHelper::GetWidgetInventoryPanel()->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleManaChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetManaPercent(NewValue, GetMaxMana());
	UDWHelper::GetWidgetInventoryPanel()->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxMana()));
}

void ADWPlayerCharacter::HandleMaxManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxManaChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetManaPercent(NewValue, GetMaxMana());
	UDWHelper::GetWidgetInventoryPanel()->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxMana()));
}

void ADWPlayerCharacter::HandleStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleStaminaChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetStaminaPercent(NewValue, GetMaxStamina());
	UDWHelper::GetWidgetInventoryPanel()->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMaxStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxStaminaChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetPrimaryPanel()->SetStaminaPercent(NewValue, GetMaxStamina());
	UDWHelper::GetWidgetInventoryPanel()->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int)NewValue, (int)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMoveSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMoveSpeedChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetMoveSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleSwimSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleSwimSpeedChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetSwimSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRideSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRideSpeedChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetRideSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleFlySpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleFlySpeedChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetFlySpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRotationSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRotationSpeedChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleJumpForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleJumpForceChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleDodgeForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDodgeForceChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleAttackForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackForceChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetAttackForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRepulseForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRepulseForceChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetRepulseForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleAttackSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackSpeedChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetAttackSpeed(FString::SanitizeFloat(NewValue, 1));
}

void ADWPlayerCharacter::HandleAttackCritRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackCritRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetAttackCritRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleAttackStealRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackStealRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetAttackStealRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetDefendRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendScopeChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendScopeChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandlePhysicsDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandlePhysicsDefRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetPhysicsDefRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleMagicDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMagicDefRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetMagicDefRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleToughnessRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleToughnessRateChanged(NewValue, DeltaValue);
	UDWHelper::GetWidgetInventoryPanel()->SetToughnessRate(FString::Printf(TEXT("%d%"), (int)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleRegenSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRegenSpeedAttribute(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleExpendSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleExpendSpeedAttribute(NewValue, DeltaValue);
}

void ADWPlayerCharacter::ToggleLockSightTarget()
{
	if (bBreakAllInput) return;

	if (!LockedTarget)
	{
		TArray<FHitResult> hitResults;
		if (UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 1000, UDWHelper::GetWorldManager()->GetGameTrace(EGameTraceType::Sight), true, TArray<AActor*>(), EDrawDebugTrace::None, hitResults, true))
		{
			ADWCharacter* nearCharacter = nullptr;
			for (int i = 0; i < hitResults.Num(); i++)
			{
				if (hitResults[i].GetActor()->IsA(ADWCharacter::StaticClass()))
				{
					ADWCharacter* character = Cast<ADWCharacter>(hitResults[i].GetActor());
					if (!character->IsDead() && (!nearCharacter || Distance(character, false, false) < Distance(nearCharacter, false, false)))
					{
						nearCharacter = character;
					}
				}
			}
			if(nearCharacter)
			{
				SetLockedTarget(nearCharacter);
			}
		}
	}
	else
	{
		SetLockedTarget(nullptr);
	}
}

void ADWPlayerCharacter::ToggleControlMode()
{
	if (bBreakAllInput || !bFreeToAnimate) return;

	if (ControlMode == EControlMode::Fighting)
		SetControlMode(EControlMode::Creating);
	else
		SetControlMode(EControlMode::Fighting);
}

void ADWPlayerCharacter::ToggleFly()
{
	if (bBreakAllInput) return;

	if (!bFalling)
	{
		Fly();
	}
	else
	{
		UnFly();
	}
}

void ADWPlayerCharacter::ToggleCrouch()
{
	if (bBreakAllInput) return;

	if (!bCrouching)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ADWPlayerCharacter::ToggleAutoJump()
{
	if (bBreakAllInput) return;

	bAutoJump = !bAutoJump;
}

void ADWPlayerCharacter::OnJumpPressed()
{
	if (bBreakAllInput) return;

	Jump();
}

void ADWPlayerCharacter::OnJumpReleased()
{
	UnJump();
}

void ADWPlayerCharacter::OnDodgePressed()
{
	if (bBreakAllInput) return;

	Dodge();
}

void ADWPlayerCharacter::OnDodgeReleased()
{
	//UnDodge();
}

void ADWPlayerCharacter::OnSprintPressed()
{
	if (bBreakAllInput) return;

	bPressedSprint = true;
}

void ADWPlayerCharacter::OnSprintReleased()
{
	bPressedSprint = false;
}

void ADWPlayerCharacter::OnAttackDestroyPressed()
{
	if (bBreakAllInput) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			if (HasWeapon())
			{
				bPressedAttack = true;
				if(bFreeToAnimate || bAttacking)
				{
					if(AttackAbilityQueue < 2)
					{
						AttackAbilityQueue++; 
					}
				}
			}
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				if (voxelHitResult.GetVoxel()->OnMouseDown(EMouseButton::Left, voxelHitResult))
				{
					DoAction(ECharacterActionType::Destroy);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnAttackDestroyRepeat()
{
	if (bBreakAllInput) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			bPressedAttack = true;
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				voxelHitResult.Voxel->OnMouseHold(EMouseButton::Left, voxelHitResult);
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnAttackDestroyReleased()
{
	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			bPressedAttack = false;
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				if (voxelHitResult.Voxel->OnMouseUp(EMouseButton::Left, voxelHitResult))
				{
					DoAction(ECharacterActionType::Generate);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGeneratePressed()
{
	if (bBreakAllInput) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			if(HasShield())
			{
				bPressedDefend = true;
			}
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				voxelHitResult.GetVoxel()->OnMouseDown(EMouseButton::Right, voxelHitResult);
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGenerateRepeat()
{
	if (bBreakAllInput) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			bPressedDefend = true;
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				voxelHitResult.Voxel->OnMouseHold(EMouseButton::Right, voxelHitResult);
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGenerateReleased()
{
	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			bPressedDefend = false;
			break;
		}
		case EControlMode::Creating:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				voxelHitResult.Voxel->OnMouseUp(EMouseButton::Right, voxelHitResult);
			}
			break;
		}
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility1()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(1))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[1]->Active();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility2()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(1))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[1]->Active();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility3()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(2))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[2]->Active();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility4()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(3))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[3]->Active();
	}
}

void ADWPlayerCharacter::ToggleInventoryPanel()
{
	UDWHelper::GetGameMode()->ToggleWidgetPanel(FName("InventoryPanel"));
}

void ADWPlayerCharacter::UseInventoryItem()
{
	UDWHelper::GetWidgetInventoryBar()->GetSelectedSlot()->UseItem(1);
}

void ADWPlayerCharacter::DiscardInventoryItem()
{
	UDWHelper::GetWidgetInventoryBar()->GetSelectedSlot()->DiscardItem(1);
}

void ADWPlayerCharacter::PrevInventorySlot()
{
	UDWHelper::GetWidgetInventoryBar()->PrevInventorySlot();
}

void ADWPlayerCharacter::NextInventorySlot()
{
	UDWHelper::GetWidgetInventoryBar()->NextInventorySlot();
}

void ADWPlayerCharacter::SelectInventorySlot1()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(0);
}

void ADWPlayerCharacter::SelectInventorySlot2()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(1);
}

void ADWPlayerCharacter::SelectInventorySlot3()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(2);
}

void ADWPlayerCharacter::SelectInventorySlot4()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(3);
}

void ADWPlayerCharacter::SelectInventorySlot5()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(4);
}

void ADWPlayerCharacter::SelectInventorySlot6()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(5);
}

void ADWPlayerCharacter::SelectInventorySlot7()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(6);
}

void ADWPlayerCharacter::SelectInventorySlot8()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(7);
}

void ADWPlayerCharacter::SelectInventorySlot9()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(8);
}

void ADWPlayerCharacter::SelectInventorySlot10()
{
	UDWHelper::GetWidgetInventoryBar()->SelectInventorySlot(9);
}

void ADWPlayerCharacter::PauseOrContinueGame()
{
	if (UDWHelper::GetGameState()->GetCurrentState() != EGameState::Pausing)
	{
		UDWHelper::GetGameMode()->PauseGame();
	}
	else
	{
		UDWHelper::GetGameMode()->ContinueGame();
	}
}

bool ADWPlayerCharacter::RaycastVoxel(FVoxelHitResult& OutHitResult)
{
	if (GetPlayerController() != nullptr)
	{
		FHitResult hitResult;
		if (GetPlayerController()->RaycastFromAimPoint(hitResult, EGameTraceType::Voxel, InteractDistance))
		{
			if (hitResult.GetActor()->IsA(AChunk::StaticClass()))
			{
				AChunk* chunk = Cast<AChunk>(hitResult.GetActor());
				if (chunk != nullptr)
				{
					UVoxel* voxel = chunk->GetVoxel(chunk->LocationToIndex(hitResult.ImpactPoint - UDWHelper::GetWorldManager()->GetBlockSizedNormal(hitResult.ImpactNormal, 0.01f)));
					if (UVoxel::IsValid(voxel))
					{
						OutHitResult = FVoxelHitResult(voxel, hitResult.ImpactPoint, hitResult.ImpactNormal);
						return true;
					}
				}
			}
		}
	}
	return false;
}

ADWPlayerCharacterController* ADWPlayerCharacter::GetPlayerController()
{
	return GetController<ADWPlayerCharacterController>();
}

ADWPlayerCharacterCameraManager* ADWPlayerCharacter::GetCameraManager()
{
	if(GetPlayerController() != nullptr)
	{
		return Cast<ADWPlayerCharacterCameraManager>(GetPlayerController()->PlayerCameraManager);
	}
	return nullptr;
}
