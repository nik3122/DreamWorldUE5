// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Character/Player/DWPlayerCharacter.h"
#include "Character/Player/DWPlayerCharacterAnim.h"
#include "Gameplay/DWPlayerController.h"
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
#include "Gameplay/DWGameMode.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widget/Components/WidgetCharacterHPComponent.h"
#include "Widget/Inventory/WidgetInventoryBar.h"
#include "Widget/Inventory/Slot/WidgetInventorySlot.h"
#include "Widget/Inventory/WidgetInventoryPanel.h"
#include "Inventory/CharacterInventory.h"
#include "Inventory/Slot/InventorySlot.h"
#include "Gameplay/DWGameState.h"
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

	TargetSystem = CreateDefaultSubobject<UTargetSystemComponent>(FName("TargetSystem"));
	TargetSystem->bShouldControlRotation = true;
	TargetSystem->TargetableActors = ADWCharacter::StaticClass();
	TargetSystem->TargetableCollisionChannel = ECC_GameTraceChannel1;
	TargetSystem->LockedOnWidgetParentSocket = FName("LockPoint");
	
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

	// states
	ControlMode = EControlMode::Fighting;
	Nature = ECharacterNature::Player;

	// stats
	VoxelItem = FVoxelItem::EmptyVoxel;

	// inputs
	bPressedAttack = false;
	bPressedDefend = false;

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

	PlayerInputComponent->BindAction("Interact1", IE_Pressed, this, &ADWPlayerCharacter::DoInteractOption1);
	PlayerInputComponent->BindAction("Interact2", IE_Pressed, this, &ADWPlayerCharacter::DoInteractOption1);
	PlayerInputComponent->BindAction("Interact3", IE_Pressed, this, &ADWPlayerCharacter::DoInteractOption1);
	PlayerInputComponent->BindAction("Interact4", IE_Pressed, this, &ADWPlayerCharacter::DoInteractOption1);
	PlayerInputComponent->BindAction("Interact5", IE_Pressed, this, &ADWPlayerCharacter::DoInteractOption1);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ADWPlayerCharacter::OnDodgePressed);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &ADWPlayerCharacter::OnDodgeReleased);

	PlayerInputComponent->BindAction("ToggleCrouch", IE_Released, this, &ADWPlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("ToggleControlMode", IE_Pressed, this, &ADWPlayerCharacter::ToggleControlMode);
	PlayerInputComponent->BindAction("ToggleLockSightTarget", IE_Pressed, this, &ADWPlayerCharacter::ToggleLockTarget);

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

	//AWorldManager::GetCurrent()->CreateTeam();
}

// Called every frame
void ADWPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

	if (bActive)
	{
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
				// FVoxelHitResult voxelHitResult;
				// if (RaycastVoxel(voxelHitResult))
				// {
				// 	voxelHitResult.GetVoxel()->OnMouseHover(voxelHitResult);
				// 	UVoxel::DespawnVoxel(voxelHitResult.GetVoxel());
				// }
				break;
			}
		}
	}
}

void ADWPlayerCharacter::LoadData(FCharacterSaveData InSaveData)
{
	Super::LoadData(InSaveData);
}

FCharacterSaveData ADWPlayerCharacter::ToData(bool bSaved)
{
	return Super::ToData(bSaved);
}

void ADWPlayerCharacter::LoadRecordData(FPlayerRecordSaveData InRecordData)
{
	SetActorLocationAndRotation(InRecordData.Location, InRecordData.Rotation);
	GetPlayerController()->SetControlRotation(GetActorRotation());
	GetPlayerController()->GetCameraManager()->SetCameraDistance(InRecordData.CamDistance, true);
}

FPlayerRecordSaveData ADWPlayerCharacter::ToRecordData(bool bSaved)
{
	FPlayerRecordSaveData PlayerRecordData;
	PlayerRecordData.bSaved = bSaved;
	PlayerRecordData.Name = GetNameC();
	PlayerRecordData.Location = GetActorLocation();
	PlayerRecordData.Rotation = GetActorRotation();
	PlayerRecordData.CamDistance = GetPlayerController()->GetCameraManager()->GetCameraDistance();
	return PlayerRecordData;
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

void ADWPlayerCharacter::Death(ADWCharacter* InKiller)
{
	Super::Death(InKiller);
	if(TargetSystem->IsLocked())
	{
		TargetSystem->TargetLockOff();
	}
}

void ADWPlayerCharacter::DeathEnd()
{
	Super::DeathEnd();
	SetInteractingTarget(this);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetCrosshairVisible(false);
}

void ADWPlayerCharacter::ResetData(bool bRefresh)
{
	Super::ResetData(bRefresh);

	ControlMode = EControlMode::Fighting;
	bPressedAttack = false;
	bPressedDefend = false;
	LockedTarget = nullptr;
	if(GetPlayerController())
	{
		GetPlayerController()->ResetData();
	}
}

void ADWPlayerCharacter::Interrupt(float InDuration /* = -1 */, bool bInPlayAnim /* = false */)
{
	Super::Interrupt(InDuration, bInPlayAnim);
	AttackAbilityQueue = 0;
}

void ADWPlayerCharacter::Dodge()
{
	if(!bDodging)
	{
		TargetSystem->SetShouldControlRotation(false);
	}
	Super::Dodge();
}

void ADWPlayerCharacter::UnDodge()
{
	if(bDodging)
	{
		TargetSystem->SetShouldControlRotation(true);
	}
	Super::UnDodge();
}

void ADWPlayerCharacter::LookAtTarget(ADWCharacter* InTargetCharacter)
{
	Super::LookAtTarget(InTargetCharacter);
	const FVector tmpDirection = InTargetCharacter->GetActorLocation() + FVector::DownVector * InTargetCharacter->GetHalfHeight() - GetActorLocation();
	const FRotator tmpRotator = tmpDirection.ToOrientationRotator();
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

void ADWPlayerCharacter::UnAttack()
{
	Super::UnAttack();
	AttackAbilityQueue = 0;
}

void ADWPlayerCharacter::AttackStart()
{
	Super::AttackStart();

	if(bAttacking)
	{
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
	const FItem tmpItem = UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryBar>()->GetSelectedItem();
	if(!VoxelItem.IsValid() || !VoxelItem.EqualType(tmpItem))
	{
		if (tmpItem.IsValid() && tmpItem.GetData().Type == EItemType::Voxel)
		{
			VoxelItem = FVoxelItem(tmpItem.ID);
			VoxelMesh->BuildVoxel(VoxelItem);
			VoxelMesh->CreateMesh(0, false);
		}
		else
		{
			VoxelMesh->ClearMesh();
			VoxelItem = FVoxelItem::EmptyVoxel;
		}
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
					const FVoxelItem& voxelItem = chunk->GetVoxelItem(chunk->LocationToIndex(hitResult.ImpactPoint - AWorldManager::GetWorldData().GetBlockSizedNormal(hitResult.ImpactNormal, 0.01f)));
					if (voxelItem.IsValid())
					{
						OutHitResult = FVoxelHitResult(voxelItem, hitResult.ImpactPoint, hitResult.ImpactNormal);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ADWPlayerCharacter::DoInteract(IInteraction* InTarget, EInteractOption InInteractOption)
{
	if(!Super::DoInteract(InTarget, InInteractOption)) return false;
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->RefreshOptions();
	return true;
}

void ADWPlayerCharacter::ToggleControlMode()
{
	if (bBreakAllInput || !IsFreeToAnim()) return;

	if (ControlMode == EControlMode::Fighting)
		SetControlMode(EControlMode::Creating);
	else
		SetControlMode(EControlMode::Fighting);
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

void ADWPlayerCharacter::ToggleLockTarget()
{
	if (bBreakAllInput) return;

	TargetSystem->TargetActor();

	// if (!LockedTarget)
	// {
	// 	TArray<FHitResult> hitResults;
	// 	if (UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 1000, UDWHelper::GetGameTrace(EGameTraceType::Sight), true, TArray<AActor*>(), EDrawDebugTrace::None, hitResults, true))
	// 	{
	// 		ADWCharacter* nearCharacter = nullptr;
	// 		for (int i = 0; i < hitResults.Num(); i++)
	// 		{
	// 			if (hitResults[i].GetActor()->IsA(ADWCharacter::StaticClass()))
	// 			{
	// 				ADWCharacter* character = Cast<ADWCharacter>(hitResults[i].GetActor());
	// 				if (!character->IsDead() && (!nearCharacter || Distance(character, false, false) < Distance(nearCharacter, false, false)))
	// 				{
	// 					nearCharacter = character;
	// 				}
	// 			}
	// 		}
	// 		if(nearCharacter)
	// 		{
	// 			SetLockedTarget(nearCharacter);
	// 		}
	// 	}
	// }
	// else
	// {
	// 	SetLockedTarget(nullptr);
	// }
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

bool ADWPlayerCharacter::UseItem(FItem& InItem)
{
	switch (InItem.GetData().Type)
	{
		case EItemType::Voxel:
		{
			if(ControlMode == EControlMode::Creating)
			{
				return Super::UseItem(InItem);
			}
			break;
		}
		case EItemType::Prop:
		{
			return Super::UseItem(InItem);
		}
		default: break;
	}
	return false;
}

void ADWPlayerCharacter::OnAttackDestroyPressed()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				voxelHitResult.GetVoxel()->OnMouseDown(EMouseButton::Left, voxelHitResult);
				// UVoxel::DespawnVoxel(voxelHitResult.GetVoxel());
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnAttackDestroyRepeat()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				if(UVoxel* voxel = voxelHitResult.GetVoxel())
				{
					voxel->OnMouseHold(EMouseButton::Left, voxelHitResult);
					UVoxel::DespawnVoxel(voxel);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnAttackDestroyReleased()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				if(UVoxel* voxel = voxelHitResult.GetVoxel())
				{
					voxel->OnMouseUp(EMouseButton::Left, voxelHitResult);
					UVoxel::DespawnVoxel(voxel);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGeneratePressed()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				if(UVoxel* voxel = voxelHitResult.GetVoxel())
				{
					voxel->OnMouseDown(EMouseButton::Right, voxelHitResult);
					UVoxel::DespawnVoxel(voxel);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGenerateRepeat()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				if(UVoxel* voxel = voxelHitResult.GetVoxel())
				{
					voxel->OnMouseHold(EMouseButton::Right, voxelHitResult);
					UVoxel::DespawnVoxel(voxel);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::OnDefendGenerateReleased()
{
	if (bBreakAllInput || UWidgetModuleBPLibrary::GetInputMode() != EInputMode::GameOnly) return;

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
				if(UVoxel* voxel = voxelHitResult.GetVoxel())
				{
					voxel->OnMouseUp(EMouseButton::Right, voxelHitResult);
					UVoxel::DespawnVoxel(voxel);
				}
			}
			break;
		}
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility1()
{
	if (bBreakAllInput) return;

	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(0))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[0]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility2()
{
	if (bBreakAllInput) return;

	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(1))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[1]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility3()
{
	if (bBreakAllInput) return;

	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(2))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[2]->ActiveItem();
	}
}

void ADWPlayerCharacter::ReleaseSkillAbility4()
{
	if (bBreakAllInput) return;

	if(Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill).IsValidIndex(3))
	{
		Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill)[3]->ActiveItem();
	}
}

void ADWPlayerCharacter::DoInteractOption1()
{
	if(!InteractingTarget || !InteractingTarget->GetInteractOptions(this).IsValidIndex(0)
		|| bBreakAllInput && InteractingTarget->GetInteractOptions(this)[0] != EInteractOption::Revive) return;

	DoInteract(InteractingTarget, InteractingTarget->GetInteractOptions(this)[0]);
}

void ADWPlayerCharacter::DoInteractOption2()
{
	if(!InteractingTarget || !InteractingTarget->GetInteractOptions(this).IsValidIndex(1)
		|| bBreakAllInput && InteractingTarget->GetInteractOptions(this)[1] != EInteractOption::Revive) return;

	DoInteract(InteractingTarget, InteractingTarget->GetInteractOptions(this)[1]);
}

void ADWPlayerCharacter::DoInteractOption3()
{
	if(!InteractingTarget || !InteractingTarget->GetInteractOptions(this).IsValidIndex(2)
		|| bBreakAllInput && InteractingTarget->GetInteractOptions(this)[2] != EInteractOption::Revive) return;
	
	DoInteract(InteractingTarget, InteractingTarget->GetInteractOptions(this)[2]);
}

void ADWPlayerCharacter::DoInteractOption4()
{
	if(!InteractingTarget || !InteractingTarget->GetInteractOptions(this).IsValidIndex(3)
		|| bBreakAllInput && InteractingTarget->GetInteractOptions(this)[3] != EInteractOption::Revive) return;

	DoInteract(InteractingTarget, InteractingTarget->GetInteractOptions(this)[3]);
}

void ADWPlayerCharacter::DoInteractOption5()
{
	if(!InteractingTarget || !InteractingTarget->GetInteractOptions(this).IsValidIndex(4)
		|| bBreakAllInput && InteractingTarget->GetInteractOptions(this)[4] != EInteractOption::Revive) return;

	DoInteract(InteractingTarget, InteractingTarget->GetInteractOptions(this)[4]);
}

void ADWPlayerCharacter::HandleNameChanged(const FString& NewValue)
{
	Super::HandleNameChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleTeamIDChanged(const FString& NewValue)
{
	Super::HandleTeamIDChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleRaceIDChanged(const FString& NewValue)
{
	Super::HandleRaceIDChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleLevelChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleLevelChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleEXPChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	Super::HandleEXPChanged(NewValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHeadInfo(GetHeadInfo());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHeadInfo(GetHeadInfo());
}

void ADWPlayerCharacter::HandleHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleHealthChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHealthPercent(NewValue, GetMaxHealth());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleMaxHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxHealthChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetHealthPercent(NewValue, GetMaxHealth());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetHealthInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxHealth()));
}

void ADWPlayerCharacter::HandleManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleManaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetManaPercent(NewValue, GetMaxMana());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxMana()));
}

void ADWPlayerCharacter::HandleMaxManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxManaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetManaPercent(NewValue, GetMaxMana());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetManaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxMana()));
}

void ADWPlayerCharacter::HandleStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleStaminaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetStaminaPercent(NewValue, GetMaxStamina());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMaxStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMaxStaminaChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->SetStaminaPercent(NewValue, GetMaxStamina());
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetStaminaInfo(FString::Printf(TEXT("%d/%d"), (int32)NewValue, (int32)GetMaxStamina()));
}

void ADWPlayerCharacter::HandleMoveSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMoveSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetMoveSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleSwimSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleSwimSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetSwimSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRideSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRideSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetRideSpeed(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleFlySpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleFlySpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetFlySpeed(FString::FromInt(NewValue));
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
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetAttackForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleRepulseForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRepulseForceChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetRepulseForce(FString::FromInt(NewValue));
}

void ADWPlayerCharacter::HandleAttackSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackSpeedChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetAttackSpeed(FString::SanitizeFloat(NewValue, 1));
}

void ADWPlayerCharacter::HandleAttackCritRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackCritRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetAttackCritRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleAttackStealRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleAttackStealRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetAttackStealRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetDefendRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleDefendScopeChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleDefendScopeChanged(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandlePhysicsDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandlePhysicsDefRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetPhysicsDefRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleMagicDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleMagicDefRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetMagicDefRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleToughnessRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleToughnessRateChanged(NewValue, DeltaValue);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetInventoryPanel>()->SetToughnessRate(FString::Printf(TEXT("%d%%"), (int32)(NewValue * 100)));
}

void ADWPlayerCharacter::HandleRegenSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleRegenSpeedAttribute(NewValue, DeltaValue);
}

void ADWPlayerCharacter::HandleExpendSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{
	Super::HandleExpendSpeedAttribute(NewValue, DeltaValue);
}

ADWPlayerController* ADWPlayerCharacter::GetPlayerController() const
{
	return GetController<ADWPlayerController>();
}

void ADWPlayerCharacter::SetInteractingTarget(IInteraction* InTarget)
{
	Super::SetInteractingTarget(InTarget);
	UWidgetModuleBPLibrary::GetUserWidget<UWidgetPrimaryPanel>()->RefreshOptions();
}
