// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterAnim.h"
#include "Character/Player/DWPlayerCharacterController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "World/Chunk.h"
#include "Voxel/Voxel.h"
#include "World/WorldManager.h"
#include "Voxel/Components/VoxelMeshComponent.h"
#include "DWGameMode.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widget/Components/WidgetCharacterHPComponent.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "DWGameState.h"
#include "ObjectPoolModuleBPLibrary.h"
#include "WidgetModuleBPLibrary.h"
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
	GetCapsuleComponent()->SetCollisionProfileName(FName("DW_Character"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->AirControl = 0.3f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0, 30, 40);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetMesh()->SetRelativeLocation(FVector(0, 0, -70));

	VoxelMesh = CreateDefaultSubobject<UVoxelMeshComponent>(FName("VoxelMesh"));
	VoxelMesh->SetupAttachment(GetMesh(), FName("VoxelMesh"));
	VoxelMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	VoxelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VoxelMesh->SetCastShadow(false);
	VoxelMesh->SetVisibility(false);
	VoxelMesh->Initialize(EVoxelMeshType::PickUp);

	HammerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("HammerMesh"));
	HammerMesh->SetupAttachment(GetMesh(), FName("HammerMesh"));
	HammerMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	HammerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HammerMesh->SetCastShadow(false);
	HammerMesh->SetVisibility(false);

	PreviewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("SceneCapture"));
	PreviewCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	PreviewCapture->OrthoWidth = 100;
	PreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	PreviewCapture->SetupAttachment(RootComponent);
	PreviewCapture->SetRelativeLocationAndRotation(FVector(100, 0, 0), FRotator(0, 180, 0));
	
	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("MiniMapCapture"));
	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MiniMapCapture->OrthoWidth = 1000;
	//MiniMapCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	MiniMapCapture->SetupAttachment(RootComponent);
	MiniMapCapture->SetRelativeLocationAndRotation(FVector(0, 0, 500), FRotator(0, 90, 0));
	
	InventoryData = FInventoryData();
	InventoryData.Items.SetNum(70);
	InventoryData.SplitInfos.Add(ESplitSlotType::Default, FSplitSlotInfo(0, 40));
	InventoryData.SplitInfos.Add(ESplitSlotType::Shortcut, FSplitSlotInfo(40, 10));
	InventoryData.SplitInfos.Add(ESplitSlotType::Auxiliary, FSplitSlotInfo(50, 1));
	InventoryData.SplitInfos.Add(ESplitSlotType::Generate, FSplitSlotInfo(51, 9));
	InventoryData.SplitInfos.Add(ESplitSlotType::Equip, FSplitSlotInfo(60, 6));
	InventoryData.SplitInfos.Add(ESplitSlotType::Skill, FSplitSlotInfo(66, 4));

	// states
	ControlMode = EControlMode::Fighting;
	Nature = ECharacterNature::Player;

	// stats
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
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->SetTickableWhenPaused(true);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADWPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADWPlayerCharacter::MoveRight);

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
}

// Called when the game starts or when spawned
void ADWPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	WidgetCharacterHP->SetWidget(nullptr);

	PreviewCapture->ShowOnlyActors.Add(this);

	//AWorldManager::GetCurrent()->CreateTeam(this);
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
	Super::LoadData(InSaveData);

	if (!InSaveData.bSaved)
	{
		Name = InSaveData.Name;
	}
}

FCharacterData ADWPlayerCharacter::ToData(bool bSaved)
{
	return Super::ToData(bSaved);
}

void ADWPlayerCharacter::LoadRecordData(FPlayerRecordData InRecordData)
{
	SetActorLocationAndRotation(InRecordData.Location, InRecordData.Rotation);
	GetPlayerController()->SetControlRotation(GetActorRotation());
	GetPlayerController()->GetCameraManager()->SetCameraDistance(InRecordData.CamDistance, true);
}

FPlayerRecordData ADWPlayerCharacter::ToRecordData(bool bSaved)
{
	FPlayerRecordData PlayerRecordData;
	PlayerRecordData.bSaved = bSaved;
	PlayerRecordData.Name = GetName();
	PlayerRecordData.Location = GetActorLocation();
	PlayerRecordData.Rotation = GetActorRotation();
	PlayerRecordData.CamDistance = GetPlayerController()->GetCameraManager()->GetCameraDistance();
	return PlayerRecordData;
}

void ADWPlayerCharacter::UnAttack()
{
	Super::UnAttack();
	AttackAbilityQueue = 0;
}

void ADWPlayerCharacter::Active(bool bResetData)
{
	Super::Active(bResetData);
}

void ADWPlayerCharacter::Disable(bool bDisableMovement, bool bDisableCollision)
{
	Super::Disable(bDisableMovement, bDisableCollision);
}

void ADWPlayerCharacter::Spawn()
{
	Super::Spawn();
	SetControlMode(ControlMode);
}

void ADWPlayerCharacter::Revive()
{
	Super::Revive();
	SetControlMode(ControlMode);
}

void ADWPlayerCharacter::ResetData(bool bRefresh)
{
	Super::ResetData(bRefresh);

	ControlMode = EControlMode::Fighting;
	bPressedAttack = false;
	bPressedDefend = false;
	bPressedSprint = false;
	LockedTarget = nullptr;
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
			if (GetWeapon()) GetWeapon()->SetVisible(true);
			if (GetShield()) GetShield()->SetVisible(true);
			break;
		}
		case EControlMode::Creating:
		{
			VoxelMesh->SetVisibility(true);
			HammerMesh->SetVisibility(true);
			if (GetWeapon()) GetWeapon()->SetVisible(false);
			if (GetShield()) GetShield()->SetVisible(false);
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
		{
			if (AttackAbilityQueue > 0)
			{
				AttackAbilityQueue--;
			}
			break;
		}
		default: break;
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
	auto tmpItem = UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>(this)->GetSelectedItem();
	if (tmpItem.IsValid() && tmpItem.GetData().Type == EItemType::Voxel)
	{
		PreviewVoxel = UVoxel::SpawnVoxel(this, tmpItem.ID);
		VoxelMesh->BuildVoxel(PreviewVoxel);
		VoxelMesh->CreateMesh(0, false);
	}
	else
	{
		VoxelMesh->ClearMesh();
		if (PreviewVoxel)
		{
			UVoxel::DespawnVoxel(this, PreviewVoxel);
			PreviewVoxel = nullptr;
		}
	}
}

void ADWPlayerCharacter::HandleNameChanged(const FString& NewValue)
{
	Super::HandleNameChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleTeamIDChanged(const FString& NewValue)
{
	Super::HandleTeamIDChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleRaceIDChanged(const FString& NewValue)
{
	Super::HandleRaceIDChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleLevelChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleLevelChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleEXPChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleEXPChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleHealthChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHealthPercent(NewValue, GetMaxHealth());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleMaxHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxHealthChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetHealthPercent(NewValue, GetMaxHealth());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleManaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetManaPercent(NewValue, GetMaxMana());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxMana()));
}

void ADWPlayerCharacter::HandleMaxManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxManaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetManaPercent(NewValue, GetMaxMana());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxMana()));
}

void ADWPlayerCharacter::HandleStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleStaminaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetStaminaPercent(NewValue, GetMaxStamina());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMaxStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxStaminaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>(this)->SetStaminaPercent(NewValue, GetMaxStamina());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMoveSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMoveSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetMoveSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleSwimSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleSwimSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetSwimSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRideSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRideSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetRideSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleFlySpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleFlySpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetFlySpeed(FString::FromInt(NewValue));
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
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetAttackForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRepulseForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRepulseForceChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetRepulseForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleAttackSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetAttackSpeed(FString::SanitizeFloat(NewValue, 1));
}

void ADWPlayerCharacter::HandleAttackCritRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackCritRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetAttackCritRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleAttackStealRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackStealRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetAttackStealRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetDefendRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendScopeChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendScopeChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandlePhysicsDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandlePhysicsDefRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetPhysicsDefRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleMagicDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMagicDefRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetMagicDefRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleToughnessRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleToughnessRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>(this)->SetToughnessRate(FString::Printf(TEXT("%d%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleRegenSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRegenSpeedAttribute(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleExpendSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleExpendSpeedAttribute(NewValue, DeltaValue);
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
					UVoxel* voxel = chunk->GetVoxel(chunk->LocationToIndex(hitResult.ImpactPoint - AWorldManager::GetData().GetBlockSizedNormal(hitResult.ImpactNormal, 0.01f)));
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

void ADWPlayerCharacter::MoveForward(float InValue)
{
	if (GetPlayerController() && !bBreakAllInput)
	{
		FVector Direction;
		const FRotator Rotation = GetPlayerController()->GetControlRotation();
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

void ADWPlayerCharacter::ToggleLockSightTarget()
{
	if (bBreakAllInput) return;

	if (!LockedTarget)
	{
		TArray<FHitResult> hitResults;
		if (UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 1000, UDWHelper::GetGameTrace(EGameTraceType::Sight), true, TArray<AActor*>(), EDrawDebugTrace::None, hitResults, true))
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

bool ADWPlayerCharacter::OnUseItem(FItem& InItem)
{
	switch (InItem.GetData().Type)
	{
		case EItemType::Voxel:
		{
			if(ControlMode == EControlMode::Creating)
			{
				return Super::OnUseItem(InItem);
			}
			break;
		}
		case EItemType::Prop:
		{
			return Super::OnUseItem(InItem);
		}
		default: break;
	}
	return false;
}

void ADWPlayerCharacter::OnAttackDestroyPressed()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode(this) != EInputMode::GameOnly) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			if (GetWeapon())
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
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode(this) != EInputMode::GameOnly) return;

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
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode(this) != EInputMode::GameOnly) return;

	switch (ControlMode)
	{
		case EControlMode::Fighting:
		{
			if(GetShield())
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
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode(this) != EInputMode::GameOnly) return;

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
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(0))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[0]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility2()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(1))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[1]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility3()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(2))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[2]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility4()
{
	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(3))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[3]->ActiveItem();
	}
}

ADWPlayerCharacterController* ADWPlayerCharacter::GetPlayerController() const
{
	return GetController<ADWPlayerCharacterController>();
}
