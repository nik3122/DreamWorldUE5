// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Character/DWCharacter.h"
#include "Character/DWCharacterAnim.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "World/WorldManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "WidgetComponent.h"
#include "AI/DWAIController.h"
#include "Widget/Components/WidgetWorldTextComponent.h"
#include "Widget/Components/WidgetCharacterHPComponent.h"
#include "Widget/Other/WidgetCharacterHP.h"
#include "Widget/Other/WidgetWorldText.h"
#include "Inventory/CharacterInventory.h"
#include "World/Chunk.h"
#include "Skill/Skill.h"
#include "AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "AIModule/Classes/Perception/AISense_Sight.h"
#include "AIModule/Classes/Perception/AISense_Damage.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Character/DWCharacterAttributeSet.h"
#include "Equip/Equip.h"
#include "Equip/EquipWeapon.h"
#include "Equip/EquipShield.h"
#include "Equip/EquipArmor.h"
#include "Inventory/Slot/InventoryEquipSlot.h"
#include "Abilities/DWGameplayAbility.h"
#include "Inventory/Slot/InventorySkillSlot.h"
#include "AIController.h"
#include "DWCharacterPart.h"
#include "DWHumanCharacter.h"
#include "GlobalToolsBPLibrary.h"
#include "PhysicsVolumeBase.h"
#include "SceneModuleBPLibrary.h"
#include "WidgetModuleBPLibrary.h"
#include "WidgetPrimaryPanel.h"
#include "AI/DWAIBlackboard.h"
#include "GameFramework/PhysicsVolume.h"
#include "Voxel/Voxel.h"
#include "Abilities/Character/DWCharacterActionAbility.h"
#include "Abilities/Character/DWCharacterAttackAbility.h"
#include "Abilities/Character/DWCharacterSkillAbility.h"
#include "Components/InteractionComponent.h"
#include "Abilities/Item/DWItemAbility.h"

//////////////////////////////////////////////////////////////////////////
// ADWCharacter

ADWCharacter::ADWCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(FName("StimuliSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Damage::StaticClass());

	Interaction = CreateDefaultSubobject<UInteractionComponent>(FName("Interaction"));
	Interaction->SetupAttachment(RootComponent);
	Interaction->SetRelativeLocation(FVector(0, 0, 0));

	WidgetCharacterHP = CreateDefaultSubobject<UWidgetCharacterHPComponent>(FName("WidgetCharacterHP"));
	WidgetCharacterHP->SetupAttachment(RootComponent);
	WidgetCharacterHP->SetRelativeLocation(FVector(0, 0, 70));

	AbilitySystem = CreateDefaultSubobject<UDWAbilitySystemComponent>(FName("AbilitySystem"));

	AttributeSet = CreateDefaultSubobject<UDWCharacterAttributeSet>(FName("AttributeSet"));

	Inventory = CreateDefaultSubobject<UCharacterInventory>(FName("Inventory"));

	AnimInstance = nullptr;
	BehaviorTree = nullptr;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DW_Character"));
	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCastShadow(false);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 360, 0);
	GetCharacterMovement()->JumpZVelocity = 420;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bComponentShouldUpdatePhysicsVolume = false;

	// states
	bDead = true;
	bDying = false;
	bActive = false;

	bFalling = false;
	bDodging = false;
	bSprinting = false;
	bCrouching = false;
	bSwimming = false;
	bFloating = false;
	bClimbing = false;
	bRiding = false;
	bFlying = false;
	bAttacking = false;
	bDefending = false;
	bDamaging = false;
	bInterrupting = false;
	bLockRotation = false;
	bFreeToAnimate = true;
	bBreakAllInput = false;

	// stats
	Name = TEXT("");
	Nature = ECharacterNature::AIHostile;
	ID = NAME_None;
	TeamID = TEXT("");
	RaceID = TEXT("");
	Level = 0;
	EXP = 0;
	BaseEXP = 100;
	EXPFactor = 2.f;
	AttackDistance = 100.f;
	InteractDistance = 500.f;
	FollowDistance = 500.f;
	PatrolDistance = 1000.f;
	PatrolDuration = 10.f;
	
	Equips = TMap<EEquipPartType, AEquip*>();
	for (int32 i = 0; i < 6; i++)
	{
		Equips.Add((EEquipPartType)i, nullptr);
	}

	MovementRate = 1;
	RotationRate = 1;
	OwnerChunk = nullptr;
	RidingTarget = nullptr;
	LockedTarget = nullptr;
	InteractingTarget = nullptr;

	InteractOptions = TArray<EInteractOption>();

	// local
	AttackAbilityIndex = 0;
	SkillAbilityIndex = NAME_None;
	AttackType = EAttackType::None;
	ActionType = ECharacterActionType::None;
	BirthLocation = FVector(0, 0, 0);
	AIMoveLocation = Vector_Empty;
	AIMoveStopDistance = 0;
	InterruptRemainTime = 0;
	NormalAttackRemainTime = 0;

	DefaultAbility = FDWAbilityData();
	AttackAbilities = TArray<FDWCharacterAttackAbilityData>();
	SkillAbilities = TMap<FName, FDWCharacterSkillAbilityData>();
	ActionAbilities = TMap<ECharacterActionType, FDWCharacterActionAbilityData>();

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = ADWAIController::StaticClass();
}

// Called when the game starts or when spawned
void ADWCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultGravityScale = GetCharacterMovement()->GravityScale;
	DefaultAirControl = GetCharacterMovement()->AirControl;

	AbilitySystem->InitAbilityActorInfo(this, this);

	AnimInstance = Cast<UDWCharacterAnim>(GetMesh()->GetAnimInstance());
	BirthLocation = GetActorLocation();

	if (GetWidgetCharacterHPWidget() && !GetWidgetCharacterHPWidget()->GetOwnerCharacter())
	{
		GetWidgetCharacterHPWidget()->SetOwnerCharacter(this);
	}

	Spawn();
}

// Called every frame
void ADWCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

	if (bActive)
	{
		Inventory->Refresh(DeltaTime);
		
		if (InterruptRemainTime != -1)
		{
			InterruptRemainTime -= DeltaTime;
			if (InterruptRemainTime <= 0)
			{
				UnInterrupt();
			}
		}

		if(NormalAttackRemainTime > 0)
		{
			NormalAttackRemainTime -= DeltaTime;
		}

		if (LockedTarget)
		{
			if (!LockedTarget->IsDead() && FVector::Distance(GetActorLocation(), LockedTarget->GetActorLocation()) <= 1000)
			{
				LookAtTarget(LockedTarget);
			}
			else
			{
				SetLockedTarget(nullptr);
			}
		}

		if (AIMoveLocation != Vector_Empty)
		{
			if (DoAIMove(AIMoveLocation, AIMoveStopDistance))
			{
				StopAIMove();
			}
		}

		const FVector Location = GetMesh()->GetSocketLocation(FName("Foot"));

		if(AWorldManager* WorldManager = AWorldManager::Get())
		{
			if(AChunk* Chunk = WorldManager->FindChunk(Location))
			{
				if(Chunk != OwnerChunk)
				{
					if(OwnerChunk)
					{
						OwnerChunk->DetachCharacter(this);
					}
					Chunk->AttachCharacter(this);
				}
			}
			else if(OwnerChunk)
			{
				OwnerChunk->DetachCharacter(this);
			}
		}

		if (bSprinting && GetMoveDirection().Size() > 0.2f)
		{
			ModifyStamina(-GetStaminaExpendSpeed() * DeltaTime);
		}
		else if(IsFreeToAnim())
		{
			ModifyStamina(GetStaminaRegenSpeed() * DeltaTime);
		}
	}

	switch (GetCharacterMovement()->MovementMode)
	{
		case EMovementMode::MOVE_Walking:
		case EMovementMode::MOVE_Swimming:
		{
			if (bFalling)
			{
				FallEnd();
			}
			break;
		}
		case EMovementMode::MOVE_Falling:
		{
			if (!bFalling)
			{
				FallStart();
			}
			break;
		}
		case EMovementMode::MOVE_Flying:
		{
			bFalling = false;
			if(GetVelocity().Z < 0.f)
			{
				FFindFloorResult FindFloorResult;
				GetCharacterMovement()->FindFloor(GetCharacterMovement()->UpdatedComponent->GetComponentLocation(), FindFloorResult, GetVelocity().IsZero(), nullptr);
				if(FindFloorResult.IsWalkableFloor())
				{
					UnFly();
				}
			}
			break;
		}
		default: break;
	}
	if (GetActorLocation().Z < 0)
	{
		Death();
	}
	if (bDying && !bFalling && ActionType != ECharacterActionType::Death)
	{
		DeathStart();
	}
}

void ADWCharacter::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if(Ar.IsSaveGame())
	{
		float CurrentValue;
		if(Ar.IsLoading())
		{
			Ar << CurrentValue;
			if(CurrentValue > 0)
			{
				SetHealth(CurrentValue);
				
				Ar << CurrentValue;
				SetMana(CurrentValue);

				Ar << CurrentValue;
				SetStamina(CurrentValue);
			}
			else
			{
				Revive();
				Ar << CurrentValue;
				Ar << CurrentValue;
			}
		}
		else if(Ar.IsSaveGame())
		{
			CurrentValue = GetHealth();
			Ar << CurrentValue;

			CurrentValue = GetMana();
			Ar << CurrentValue;

			CurrentValue = GetStamina();
			Ar << CurrentValue;
		}
	}
}

void ADWCharacter::LoadData(FCharacterSaveData InSaveData)
{
	if (InSaveData.bSaved)
	{
		ID = InSaveData.ID;
		Nature = InSaveData.Nature;
		SetNameC(InSaveData.Name);
		SetRaceID(InSaveData.RaceID);
		SetTeamID(InSaveData.TeamID);
		SetLevelC(InSaveData.Level);
		SetEXP(InSaveData.EXP);
		AttackDistance = InSaveData.AttackDistance;
		InteractDistance = InSaveData.InteractDistance;
		FollowDistance = InSaveData.FollowDistance;
		PatrolDistance = InSaveData.PatrolDistance;
		PatrolDuration = InSaveData.PatrolDuration;

		DefaultAbility = InSaveData.DefaultAbility;
		FallingAttackAbility = InSaveData.FallingAttackAbility;
		AttackAbilities = InSaveData.AttackAbilities;
		SkillAbilities = InSaveData.SkillAbilities;
		ActionAbilities = InSaveData.ActionAbilities;

		Inventory->LoadData(InSaveData.InventoryData, this);

		SetActorLocation(InSaveData.SpawnLocation);
		SetActorRotation(InSaveData.SpawnRotation);

		for(auto& iter : AttackAbilities)
		{
			iter.AbilityHandle = AcquireAbility(iter.AbilityClass, iter.AbilityLevel);
		}
		
		for(auto& iter : SkillAbilities)
		{
			iter.Value.AbilityHandle = AcquireAbility(iter.Value.AbilityClass, iter.Value.AbilityLevel);
		}

		for(auto& iter : ActionAbilities)
		{
			iter.Value.AbilityHandle = AcquireAbility(iter.Value.AbilityClass, iter.Value.AbilityLevel);
		}

		FallingAttackAbility.AbilityHandle = AcquireAbility(FallingAttackAbility.AbilityClass, FallingAttackAbility.AbilityLevel);

		DefaultAbility.AbilityHandle = AcquireAbility(GetCharacterData().AbilityClass, DefaultAbility.AbilityLevel);
		ActiveAbility(DefaultAbility.AbilityHandle);

		UGlobalToolsBPLibrary::LoadObjectFromMemory(this, InSaveData.Datas);
	}
	else
	{
		ID = InSaveData.ID;
		SetNameC(InSaveData.Name);
		SetRaceID(InSaveData.RaceID);
		SetLevelC(InSaveData.Level);

		SetActorLocation(InSaveData.SpawnLocation);
		SetActorRotation(InSaveData.SpawnRotation);

		const FCharacterData characterData = GetCharacterData();
		if(characterData.IsValid())
		{
			FString contextString;
			
			if (characterData.AttackAbilityTable != nullptr)
			{
				TArray<FDWCharacterAttackAbilityData*> attackAbilities;
				characterData.AttackAbilityTable->GetAllRows(contextString, attackAbilities);
				for (int i = 0; i < attackAbilities.Num(); i++)
				{
					auto abilityData = *attackAbilities[i];
					abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
					AttackAbilities.Add(abilityData);
				}
			}

			if (characterData.SkillAbilityTable != nullptr)
			{
				TArray<FDWCharacterSkillAbilityData*> skillAbilities;
				characterData.SkillAbilityTable->GetAllRows(contextString, skillAbilities);
				for (int i = 0; i < skillAbilities.Num(); i++)
				{
					auto abilityData = *skillAbilities[i];
					abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
					SkillAbilities.Add(abilityData.AbilityName, abilityData);
				}
			}

			if (characterData.ActionAbilityTable != nullptr)
			{
				TArray<FDWCharacterActionAbilityData*> actionAbilities;
				characterData.ActionAbilityTable->GetAllRows(contextString, actionAbilities);
				for (int i = 0; i < actionAbilities.Num(); i++)
				{
					auto abilityData = *actionAbilities[i];
					abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
					ActionAbilities.Add(abilityData.ActionType, abilityData);
				}
			}

			if (characterData.FallingAttackAbility.AbilityClass != nullptr)
			{
				FallingAttackAbility = characterData.FallingAttackAbility;
				FallingAttackAbility.AbilityHandle = AcquireAbility(FallingAttackAbility.AbilityClass, FallingAttackAbility.AbilityLevel);
			}

			if(characterData.AbilityClass != nullptr)
			{
				DefaultAbility = FDWAbilityData();
				DefaultAbility.AbilityName = FName("Default");
				DefaultAbility.AbilityLevel = InSaveData.Level;
				DefaultAbility.AbilityHandle = AcquireAbility(characterData.AbilityClass, DefaultAbility.AbilityLevel);
				ActiveAbility(DefaultAbility.AbilityHandle);
			}

			Nature = characterData.Nature;
			AttackDistance = characterData.AttackDistance;
			InteractDistance = characterData.InteractDistance;
			FollowDistance = characterData.FollowDistance;
			PatrolDistance = characterData.PatrolDistance;
			PatrolDuration = characterData.PatrolDuration;
			
			InSaveData.InventoryData = characterData.InventoryData;
		}

		// if(!IsPlayer())
		// {
		// 	const auto ItemDatas = UDWHelper::LoadItemDatas();
		// 	if(ItemDatas.Num() > 0 && FMath::FRand() < 0.5f)
		// 	{
		// 		InSaveData.InventoryData.Items.Add(FItem(ItemDatas[FMath::RandRange(0, ItemDatas.Num() - 1)].ID, 1));
		// 	}
		// }

		Inventory->LoadData(InSaveData.InventoryData, this);
	}
}

FCharacterSaveData ADWCharacter::ToData(bool bSaved)
{
	auto SaveData = FCharacterSaveData();

	SaveData.bSaved = bSaved;

	SaveData.ID = ID;
	SaveData.Nature = Nature;
	SaveData.Name = Name;
	SaveData.TeamID = TeamID;
	SaveData.RaceID = RaceID;
	SaveData.Level = Level;
	SaveData.EXP = EXP;
	SaveData.AttackDistance = AttackDistance;
	SaveData.InteractDistance = InteractDistance;
	SaveData.FollowDistance = FollowDistance;
	SaveData.PatrolDistance = PatrolDistance;
	SaveData.PatrolDuration = PatrolDuration;

	SaveData.InventoryData = Inventory->ToData();

	SaveData.DefaultAbility = DefaultAbility;
	SaveData.FallingAttackAbility = FallingAttackAbility;
	SaveData.AttackAbilities = AttackAbilities;
	SaveData.SkillAbilities = SkillAbilities;
	SaveData.ActionAbilities = ActionAbilities;

	SaveData.SpawnLocation = GetActorLocation();
	SaveData.SpawnRotation = GetActorRotation();

	UGlobalToolsBPLibrary::SaveObjectToMemory(this, SaveData.Datas);

	return SaveData;
}

void ADWCharacter::Active(bool bResetData /*= false*/)
{
	if (!bActive)
	{
		bActive = true;
		UnInterrupt();
		GetCharacterMovement()->SetActive(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (bResetData) ResetData();
}

void ADWCharacter::Disable(bool bDisableMovement, bool bDisableCollision)
{
	if (bActive)
	{
		bActive = false;
		Interrupt();
		if (bDisableMovement)
		{
			GetCharacterMovement()->SetActive(false);
		}
		if (bDisableCollision)
		{
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ADWCharacter::Spawn()
{
	Active(true);
	SetVisible(true);
	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
	SetStamina(GetMaxStamina());
	DoAction(ECharacterActionType::Revive);
}

void ADWCharacter::Revive()
{
	if (bDead && !bDying)
	{
		Active(true);
		SetVisible(true);
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
		DoAction(ECharacterActionType::Revive);
	}
}

void ADWCharacter::Death(ADWCharacter* InKiller /*= nullptr*/)
{
	if (!IsDead())
	{
		bDying = true;
		Disable(true);
		if(InKiller != nullptr)
		{
			InKiller->ModifyEXP(GetTotalEXP());
		}
		SetEXP(0);
		SetHealth(0.f);
		SetMana(0.f);
		SetStamina(0.f);
		SetLockedTarget(nullptr);
		InteractingTarget = nullptr;
		OnCharacterDead.Broadcast();
		if (!IsPlayer())
		{
			GetTeamData()->RemoveMember(this);
			GetController<ADWAIController>()->UnPossess();
		}
	}
}

void ADWCharacter::DeathStart()
{
	DoAction(ECharacterActionType::Death);
}

void ADWCharacter::DeathEnd()
{
	bDead = true;
	bDying = false;
	SetVisible(false);
	GetCharacterMovement()->SetActive(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(Inventory) Inventory->DiscardAll();
	if(!IsPlayer())
	{
		if(GetController())
		{
			GetController()->UnPossess();
		}
		if (OwnerChunk && OwnerChunk->IsValidLowLevel())
		{
			OwnerChunk->DestroyCharacter(this);
		}
		else
		{
			Destroy();
		}
		if (HasTeam())
		{
			GetTeamData()->RemoveMember(this);
		}
		for(auto Iter : Equips)
		{
			if(Iter.Value)
			{
				Iter.Value->Destroy();
			}
		}
	}
}

bool ADWCharacter::DoInteract(IInteraction* InTarget, EInteractOption InInteractOption)
{
	if(!InTarget) return false;

	return InTarget->OnInteract(this, InInteractOption);
}

bool ADWCharacter::OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption)
{
	if (!InTrigger) return false;

	if(GetInteractOptions(InTrigger).Contains(InInteractOption))
	{
		switch (InInteractOption)
		{
			case EInteractOption::Revive:
			{
				if(bDead)
				{
					Revive();
				}
			}
			default: break;
		}
		return true;
	}
	return false;
}

void ADWCharacter::ResetData(bool bRefresh)
{
	// states
	bDead = false;
	bDying = false;
	bDodging = false;
	bSprinting = false;
	bCrouching = false;
	bSwimming = false;
	bFloating = false;
	bAttacking = false;
	bDefending = false;
	bDamaging = false;

	// stats
	SetMotionRate(1, 1);
	SetLockedTarget(nullptr);
	SetInteractingTarget(nullptr);
	OwnerRider = nullptr;
	RidingTarget = nullptr;
			
	// local
	AttackAbilityIndex = 0;
	AIMoveLocation = Vector_Empty;
	AIMoveStopDistance = 0;
	InterruptRemainTime = 0;
	NormalAttackRemainTime = 0;
	ActionType = ECharacterActionType::None;

	if(bRefresh) RefreshData();
}

void ADWCharacter::RefreshData()
{
	HandleEXPChanged(GetEXP());
	HandleLevelChanged(GetLevelC());
	HandleHealthChanged(GetHealth());
	HandleManaChanged(GetMana());
	HandleStaminaChanged(GetStamina());
	HandleMoveSpeedChanged(GetMoveSpeed());
	HandleSwimSpeedChanged(GetSwimSpeed());
	HandleRideSpeedChanged(GetRideSpeed());
	HandleFlySpeedChanged(GetFlySpeed());
	HandleJumpForceChanged(GetJumpForce());
	HandleDodgeForceChanged(GetDodgeForce());
	HandleAttackForceChanged(GetAttackForce());
	HandleRepulseForceChanged(GetRepulseForce());
	HandleAttackSpeedChanged(GetAttackSpeed());
	HandleAttackCritRateChanged(GetAttackCritRate());
	HandleAttackStealRateChanged(GetAttackStealRate());
	HandleDefendRateChanged(GetDefendRate());
	HandlePhysicsDefRateChanged(GetPhysicsDefRate());
	HandleMagicDefRateChanged(GetMagicDefRate());
	HandleToughnessRateChanged(GetToughnessRate());
}

void ADWCharacter::FreeToAnim(bool bUnLockRotation /*= true*/)
{
	if (!bInterrupting)
	{
		bFreeToAnimate = true;
	}
	if (bUnLockRotation)
	{
		bLockRotation = false;
	}
}

void ADWCharacter::LimitToAnim(bool bInLockRotation /*= false*/, bool bUnSprint /*= false*/)
{
	if (!bFlying && !bFalling && !bRiding && !bClimbing && !bDefending)
	{
		bFreeToAnimate = false;
	}
	if (bInLockRotation)
	{
		bLockRotation = true;
	}
	if (bUnSprint)
	{
		UnSprint();
	}
}

void ADWCharacter::Interrupt(float InDuration /*= -1*/, bool bDoAction /*= false*/)
{
	if (!bInterrupting)
	{
		UnFly();
		UnRide();
		UnClimb();
		UnJump();
		UnDodge();
		UnAttack();
		UnDefend();
		UnSprint();
		LimitToAnim();
		bInterrupting = true;
		bBreakAllInput = true;
	}
	if (InDuration != -1) InDuration = InDuration * (1 - GetToughnessRate());
	InterruptRemainTime = InDuration;
	if (bDoAction) DoAction(ECharacterActionType::Interrupt);
}

void ADWCharacter::UnInterrupt()
{
	if (bInterrupting)
	{
		bInterrupting = false;
		bBreakAllInput = false;
		FreeToAnim();
		StopAction(ECharacterActionType::Interrupt);
	}
}

void ADWCharacter::Jump()
{
	if (IsFreeToAnim() && !bSwimming)
	{
		if(DoAction(ECharacterActionType::Jump))
		{
			Super::Jump();
		}
	}
}

void ADWCharacter::UnJump()
{
	if(bFalling)
	{
		StopAction(ECharacterActionType::Jump);
		Super::StopJumping();
	}
}

void ADWCharacter::Dodge()
{
	if (!bDodging && IsFreeToAnim() && GetMoveDirection().Size() > 0)
	{
		if (DoAction(ECharacterActionType::Dodge))
		{
			bDodging = true;
			LimitToAnim();
			GetCapsuleComponent()->SetGenerateOverlapEvents(false);
			SetActorRotation(FRotator(0.f, GetMoveDirection().ToOrientationRotator().Yaw, 0.f));
		}
	}
}

void ADWCharacter::UnDodge()
{
	if (bDodging)
	{
		bDodging = false;
		FreeToAnim();
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
		StopAction(ECharacterActionType::Dodge);
	}
}

void ADWCharacter::Sprint()
{
	if (!bSprinting && IsFreeToAnim())
	{
		bSprinting = true;
		HandleMoveSpeedChanged(GetMoveSpeed());
	}
}

void ADWCharacter::UnSprint()
{
	if (bSprinting)
	{
		bSprinting = false;
		HandleMoveSpeedChanged(GetMoveSpeed());
	}
}

void ADWCharacter::Crouch(bool bClientSimulation /*= false*/)
{
	if (!bCrouching && DoAction(ECharacterActionType::Crouch))
	{
		bCrouching = true;
		LimitToAnim();
		Super::Crouch(bClientSimulation);
	}
}

void ADWCharacter::UnCrouch(bool bClientSimulation /*= false*/)
{
	if (bCrouching)
	{
		bCrouching = false;
		FreeToAnim();
		StopAction(ECharacterActionType::Crouch);
		Super::UnCrouch(bClientSimulation);
	}
}

void ADWCharacter::Swim()
{
	if(!bSwimming && DoAction(ECharacterActionType::Swim))
	{
		bSwimming = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
 		if(USceneModuleBPLibrary::HasPhysicsVolume(FName("Water")))
		{
			if(GetCharacterMovement()->UpdatedComponent)
			{
				GetCharacterMovement()->UpdatedComponent->SetPhysicsVolume(USceneModuleBPLibrary::GetPhysicsVolume(FName("Water")), true);
			}
		}
		//FVector Velocity = GetMovementComponent()->Velocity;
		//GetMovementComponent()->Velocity = FVector(Velocity.X, Velocity.Y, 0);
	}
}

void ADWCharacter::UnSwim()
{
	if (bSwimming)
	{
		bSwimming = false;
		UnFloat();
		StopAction(ECharacterActionType::Swim);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if(GetCharacterMovement()->UpdatedComponent)
		{
			GetCharacterMovement()->UpdatedComponent->SetPhysicsVolume(GetWorld()->GetDefaultPhysicsVolume(), true);
		}
		//FVector Velocity = GetMovementComponent()->Velocity;
		//GetMovementComponent()->Velocity = FVector(Velocity.X, Velocity.Y, 0);
	}
}

void ADWCharacter::Float(float InWaterPosZ)
{
	if(!bFloating && bSwimming)
	{
		bFloating = true;
		GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0);
		const float NeckPosZ = GetCharacterPart(ECharacterPartType::Neck)->GetComponentLocation().Z;
		const float ChestPosZ = GetCharacterPart(ECharacterPartType::Chest)->GetComponentLocation().Z;
		const float OffsetZ = ChestPosZ + (NeckPosZ - ChestPosZ) * 0.35f - GetActorLocation().Z;
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, InWaterPosZ - OffsetZ));
	}
}

void ADWCharacter::UnFloat()
{
	if(bFloating)
	{
		bFloating = false;
		GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, GetCharacterMovement()->Velocity.Z * 0.5f);
	}
}

void ADWCharacter::Climb()
{
	if (!bClimbing && DoAction(ECharacterActionType::Climb))
	{
		bClimbing = true;
		LimitToAnim();
	}
}

void ADWCharacter::UnClimb()
{
	if (bClimbing)
	{
		bClimbing = false;
		FreeToAnim();
		StopAction(ECharacterActionType::Climb);
	}
}

void ADWCharacter::Ride(ADWCharacter* InTarget)
{
	if (!bRiding && InTarget && DoAction(ECharacterActionType::Ride))
	{
		bRiding = true;
		RidingTarget = InTarget;
		if(GetOwnerController())
		{
			GetOwnerController()->Possess(RidingTarget);
		}
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttachToComponent(RidingTarget->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("RiderPoint"));
		SetActorRelativeLocation(FVector::ZeroVector);
		SetActorRotation(RidingTarget->GetActorRotation());
		LimitToAnim();
	}
}

void ADWCharacter::UnRide()
{
	if (bRiding)
	{
		bRiding = false;
		FreeToAnim();
		if(IsActive()) GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if(RidingTarget)
		{
			if(RidingTarget->GetOwnerController())
			{
				RidingTarget->GetOwnerController()->Possess(RidingTarget);
			}
			FHitResult hitResult;
			const FVector offset = GetActorRightVector() * (GetRadius() + RidingTarget->GetRadius());
			const FVector rayStart = FVector(GetActorLocation().X, GetActorLocation().Y, AWorldManager::GetWorldData().ChunkHeightRange * AWorldManager::GetWorldData().GetChunkLength() + 500) + offset;
			const FVector rayEnd = FVector(GetActorLocation().X, GetActorLocation().Y, 0) + offset;
			if (AWorldManager::Get()->ChunkTraceSingle(rayStart, rayEnd, GetRadius(), GetHalfHeight(), hitResult))
			{
				SetActorLocation(hitResult.Location);
			}
		}
		RidingTarget = nullptr;
		GetOwnerController()->Possess(this);
		StopAction(ECharacterActionType::Ride);
	}
}

void ADWCharacter::Fly()
{
	if (!bFlying && DoAction(ECharacterActionType::Fly))
	{
		bFlying = true;
		LimitToAnim();
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 100.f);
		// GetCharacterMovement()->GravityScale = 0.f;
		// GetCharacterMovement()->AirControl = 1.f;
	}
}

void ADWCharacter::UnFly()
{
	if (bFlying)
	{
		bFlying = false;
		FreeToAnim();
		StopAction(ECharacterActionType::Fly);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// GetCharacterMovement()->GravityScale = DefaultGravityScale;
		// GetCharacterMovement()->AirControl = DefaultAirControl;
	}
}

bool ADWCharacter::Attack(int32 InAbilityIndex /*= -1*/)
{
	if (IsFreeToAnim() && NormalAttackRemainTime <= 0.f)
	{
		if(InAbilityIndex == -1) InAbilityIndex = AttackAbilityIndex;
		if (HasAttackAbility(InAbilityIndex) && HasWeapon(GetAttackAbility(InAbilityIndex).WeaponType))
		{
			if (ActiveAbility(GetAttackAbility(InAbilityIndex).AbilityHandle))
			{
				bAttacking = true;
				LimitToAnim(true, true);
				SetMotionRate(0, 0);
				AttackAbilityIndex = InAbilityIndex;
				AttackType = EAttackType::NormalAttack;
				NormalAttackRemainTime = 1.f / GetAttackSpeed();
				return true;
			}
			else
			{
				UnAttack();
			}
		}
	}
	else if(!bAttacking && bFalling)
	{
		return FallingAttack();
	}
	return false;
}

bool ADWCharacter::SkillAttack(const FName& InSkillID)
{
	if (IsFreeToAnim())
	{
		if (HasSkillAbility(InSkillID))
		{
			const auto AbilityData = GetSkillAbility(InSkillID);
			switch(AbilityData.GetItemData().SkillMode)
			{
				case ESkillMode::Initiative:
				{
					if(AbilityData.WeaponType == EWeaponType::None || AbilityData.WeaponType == GetWeapon()->GetWeaponData().WeaponType)
					{
						if(ActiveAbility(AbilityData.AbilityHandle))
						{
							bAttacking = true;
							LimitToAnim(true, true);
							SetMotionRate(0, 0);
							SkillAbilityIndex = InSkillID;
							AttackType = EAttackType::SkillAttack;
							return true;
						}
					}
					break;
				}
				case ESkillMode::Passive:
				{
					return true;
				}
				default: break;
			}
		}
	}
	return false;
}

bool ADWCharacter::SkillAttack(ESkillType InSkillType, int32 InAbilityIndex)
{
	if (IsFreeToAnim())
	{
		if (HasSkillAbility(InSkillType, InAbilityIndex))
		{
			const auto AbilityData = GetSkillAbility(InSkillType, InAbilityIndex);
			SkillAttack(AbilityData.AbilityName);
		}
	}
	return false;
}

bool ADWCharacter::FallingAttack()
{
	if (FallingAttackAbility.AbilityClass != nullptr && HasWeapon(FallingAttackAbility.WeaponType))
	{
		if (ActiveAbility(FallingAttackAbility.AbilityHandle))
		{
			bAttacking = true;
			LimitToAnim(true, true);
			SetMotionRate(0, 0);
			AttackType = EAttackType::FallingAttack;
			return true;
		}
	}
	return false;
}

void ADWCharacter::AttackStart()
{
	if (bAttacking)
	{
		switch (AttackType)
		{
			case EAttackType::NormalAttack:
			case EAttackType::FallingAttack:
			{
				SetDamaging(true);
				break;
			}
			case EAttackType::SkillAttack:
			{
				if (GetSkillAbility(SkillAbilityIndex).GetItemData().SkillClass != nullptr)
				{
					FActorSpawnParameters spawnParams = FActorSpawnParameters();
					spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					ASkill* tmpSkill = GetWorld()->SpawnActor<ASkill>(GetSkillAbility(SkillAbilityIndex).GetItemData().SkillClass, spawnParams);
					if(tmpSkill) tmpSkill->Initialize(this, SkillAbilityIndex);
				}
				break;
			}
			default: break;
		}
	}
}

void ADWCharacter::AttackHurt()
{
	if (bAttacking)
	{
		SetDamaging(false);
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this](){ SetDamaging(true); });
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

void ADWCharacter::AttackEnd()
{
	if (bAttacking)
	{
		switch (AttackType)
		{
			case EAttackType::NormalAttack:
			{
				SetDamaging(false);
				if (++AttackAbilityIndex >= AttackAbilities.Num())
				{
					AttackAbilityIndex = 0;
				}
				break;
			}
			case EAttackType::SkillAttack:
			{
				UnAttack();
				break;
			}
			case EAttackType::FallingAttack:
			{
				UnAttack();
				StopAnimMontage();
				break;
			}
			default: break;
		}
	}
}

void ADWCharacter::UnAttack()
{
	if (bAttacking)
	{
		bAttacking = false;
		FreeToAnim();
		SetMotionRate(1, 1);
		SetDamaging(false);
		AttackAbilityIndex = 0;
		SkillAbilityIndex = NAME_None;
		AttackType = EAttackType::None;
	}
}

void ADWCharacter::Defend()
{
	if (IsFreeToAnim() && DoAction(ECharacterActionType::Defend))
	{
		bDefending = true;
		SetMotionRate(0.5f, 0.1f);
		LimitToAnim(true, true);
	}
}

void ADWCharacter::UnDefend()
{
	if (bDefending)
	{
		bDefending = false;
		FreeToAnim();
		SetMotionRate(1, 1);
		StopAction(ECharacterActionType::Defend);
	}
}

void ADWCharacter::FallStart()
{
	bFalling = true;
	if(bDefending)
	{
		UnDefend();
	}
}

void ADWCharacter::FallEnd()
{
	bFalling = false;
	UnJump();
	if(bAttacking && AttackType == EAttackType::FallingAttack)
	{
		AttackEnd();
	}
}

bool ADWCharacter::UseItem(FItem& InItem)
{
	switch (InItem.GetData().Type)
	{
		case EItemType::Voxel:
		{
			FVoxelHitResult voxelHitResult;
			if (RaycastVoxel(voxelHitResult))
			{
				return GenerateVoxel(voxelHitResult, InItem);
			}
			break;
		}
		case EItemType::Prop:
		{
			const FPropData PropData = UDWHelper::LoadPropData(InItem.ID);
			switch (PropData.PropType)
			{
				case EPropType::Potion:
				{
					if(PropData.Name.ToString().StartsWith(TEXT("生命")))
					{
						return GetHealth() < GetMaxHealth();
					}
					else if(PropData.Name.ToString().StartsWith(TEXT("魔法")))
					{
						return GetMana() < GetMaxMana();
					}
					else if(PropData.Name.ToString().StartsWith(TEXT("体力")))
					{
						return GetStamina() < GetMaxStamina();
					}
					break;
				}
				case EPropType::Food:
				{
					return GetHealth() < GetMaxHealth() || GetStamina() < GetMaxStamina();
				}
				default: break;
			}
			break;
		}
		default: break;
	}
	return false;
}

bool ADWCharacter::GenerateVoxel(const FVoxelHitResult& InVoxelHitResult, FItem& InItem)
{
	if(DoAction(ECharacterActionType::Generate))
	{
		AChunk* chunk = InVoxelHitResult.GetOwner();
		const FIndex index = chunk->LocationToIndex(InVoxelHitResult.Point - AWorldManager::GetWorldData().GetBlockSizedNormal(InVoxelHitResult.Normal)) + FIndex(InVoxelHitResult.Normal);
		const FVoxelItem& voxelItem = chunk->GetVoxelItem(index);

		if(!voxelItem.IsValid() || voxelItem.GetVoxelData().Transparency == ETransparency::Transparent && voxelItem != InVoxelHitResult.VoxelItem)
		{
			const FVoxelItem _voxelItem = FVoxelItem(InItem.ID);

			//FRotator rotation = (Owner->VoxelIndexToLocation(index) + tmpVoxel->GetVoxelData().GetCeilRange() * 0.5f * AWorldManager::GetWorldInfo().BlockSize - UDWHelper::GetPlayerCharacter(this)->GetActorLocation()).ToOrientationRotator();
			//rotation = FRotator(FRotator::ClampAxis(FMath::RoundToInt(rotation.Pitch / 90) * 90.f), FRotator::ClampAxis(FMath::RoundToInt(rotation.Yaw / 90) * 90.f), FRotator::ClampAxis(FMath::RoundToInt(rotation.Roll / 90) * 90.f));
			//tmpVoxel->Rotation = rotation;

			FHitResult HitResult;
			if (!AWorldManager::Get()->VoxelTraceSingle(_voxelItem, chunk->IndexToLocation(index), HitResult))
			{
				if(voxelItem.IsValid())
				{
					return chunk->ReplaceVoxel(voxelItem, _voxelItem);
				}
				else
				{
					return chunk->GenerateVoxel(index, _voxelItem);
				}
			}
		}
	}
	return false;
}

bool ADWCharacter::DestroyVoxel(const FVoxelHitResult& InVoxelHitResult)
{
	if(DoAction(ECharacterActionType::Destroy))
	{
		AChunk* chunk = InVoxelHitResult.GetOwner();
		const FVoxelItem& voxelItem = InVoxelHitResult.VoxelItem;

		if (voxelItem.GetVoxelData().VoxelType != EVoxelType::Bedrock)
		{
			return chunk->DestroyVoxel(voxelItem);
		}
	}
	return false;
}

void ADWCharacter::RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot)
{
	if (!EquipSlot->IsEmpty())
	{
		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Equips[InPartType] = GetWorld()->SpawnActor<AEquip>(UDWHelper::LoadEquipData(EquipSlot->GetItem().ID).EquipClass, spawnParams);
		if (Equips[InPartType])
		{
			Equips[InPartType]->Initialize(this);
			Equips[InPartType]->OnAssemble();
		}
	}
	else if(Equips[InPartType])
	{
		Equips[InPartType]->OnDischarge();
		Equips[InPartType]->Destroy();
		Equips[InPartType] = nullptr;
	}
}

FGameplayAbilitySpecHandle ADWCharacter::AcquireAbility(TSubclassOf<UDWGameplayAbility> InAbility, int32 InLevel /*= 1*/)
{
	if (AbilitySystem && InAbility)
	{
		FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
		SpecDef.Ability = InAbility;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, InLevel);
		return AbilitySystem->GiveAbility(AbilitySpec);
	}
	return FGameplayAbilitySpecHandle();
}

bool ADWCharacter::ActiveAbility(FGameplayAbilitySpecHandle SpecHandle, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbility(SpecHandle, bAllowRemoteActivation);
	}
	return false;
}

bool ADWCharacter::ActiveAbility(TSubclassOf<UDWGameplayAbility> AbilityClass, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbilityByClass(AbilityClass, bAllowRemoteActivation);
	}
	return false;
}

bool ADWCharacter::ActiveAbility(const FGameplayTagContainer& AbilityTags, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}
	return false;
}

void ADWCharacter::CancelAbility(UDWGameplayAbility* Ability)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbility(Ability);
	}
}

void ADWCharacter::CancelAbility(const FGameplayAbilitySpecHandle& AbilityHandle)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbilityHandle(AbilityHandle);
	}
}

void ADWCharacter::CancelAbilities(const FGameplayTagContainer* WithTags/*=nullptr*/, const FGameplayTagContainer* WithoutTags/*=nullptr*/, UDWGameplayAbility* Ignore/*=nullptr*/)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbilities(WithTags, WithoutTags, Ignore);
	}
}

void ADWCharacter::CancelAllAbilities(UDWGameplayAbility* Ignore/*=nullptr*/)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAllAbilities(Ignore);
	}
}

FActiveGameplayEffectHandle ADWCharacter::ApplyEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (AbilitySystem)
	{
		auto effectContext = AbilitySystem->MakeEffectContext();
		effectContext.AddSourceObject(this);
		auto specHandle = AbilitySystem->MakeOutgoingSpec(EffectClass, GetLevelC(), effectContext);
		if (specHandle.IsValid())
		{
			return AbilitySystem->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
		}
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle ADWCharacter::ApplyEffect(const FGameplayEffectSpecHandle& SpecHandle)
{
	if (AbilitySystem)
	{
		return AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle ADWCharacter::ApplyEffect(const FGameplayEffectSpec& GameplayEffect)
{
	if (AbilitySystem)
	{
		return AbilitySystem->ApplyGameplayEffectSpecToSelf(GameplayEffect);
	}
	return FActiveGameplayEffectHandle();
}

bool ADWCharacter::RemoveEffect(FActiveGameplayEffectHandle Handle, int32 StacksToRemove/*=-1*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->RemoveActiveGameplayEffect(Handle, StacksToRemove);
	}
	return false;
}

void ADWCharacter::GetActiveAbilities(FGameplayTagContainer AbilityTags, TArray<UDWGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystem)
	{
		AbilitySystem->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ADWCharacter::GetAbilityInfo(TSubclassOf<UDWGameplayAbility> AbilityClass, FDWAbilityInfo& OutAbilityInfo)
{
	if (AbilitySystem && AbilityClass != nullptr)
	{
		float Cost = 0;
		float Cooldown = 0;
		EAbilityCostType CostType = EAbilityCostType::None;
		UDWGameplayAbility* Ability = AbilityClass.GetDefaultObject();
		if (Ability->GetCostGameplayEffect()->Modifiers.Num() > 0)
		{
			const FGameplayModifierInfo ModifierInfo = Ability->GetCostGameplayEffect()->Modifiers[0];
			ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
			if (ModifierInfo.Attribute == AttributeSet->GetHealthAttribute())
			{
				CostType = EAbilityCostType::Health;
			}
			else if (ModifierInfo.Attribute == AttributeSet->GetManaAttribute())
			{
				CostType = EAbilityCostType::Mana;
			}
			else if (ModifierInfo.Attribute == AttributeSet->GetStaminaAttribute())
			{
				CostType = EAbilityCostType::Stamina;
			}
		}
		Ability->GetCooldownGameplayEffect()->DurationMagnitude.GetStaticMagnitudeIfPossible(1, Cooldown);
		OutAbilityInfo = FDWAbilityInfo(CostType, Cost, Cooldown);
		return true;
	}
	return false;
}

bool ADWCharacter::DoAction(ECharacterActionType InActionType)
{
	if(!HasActionAbility(InActionType)) return false;

	const FDWCharacterActionAbilityData AbilityData = GetActionAbility(InActionType);
	if(!AbilityData.bNeedActive || IsActive(AbilityData.bNeedFreeToAnim))
	{
		if (StopAction(ActionType, true, true) && ActiveAbility(AbilityData.AbilityHandle))
		{
			ActionType = InActionType;
			return true;
		}
	}
	return false;
}

void ADWCharacter::EndAction(ECharacterActionType InActionType)
{
	switch(InActionType)
	{
		case ECharacterActionType::Death:
		{
			DeathEnd();
			break;
		}
		case ECharacterActionType::Jump:
		{
			UnJump();
			break;
		}
		case ECharacterActionType::Dodge:
		{
			UnDodge();
			break;
		}
		case ECharacterActionType::Crouch:
		{
			UnCrouch();
			break;
		}
		case ECharacterActionType::Defend:
		{
			UnDefend();
			break;
		}
		case ECharacterActionType::Fly:
		{
			UnFly();
			break;
		}
		case ECharacterActionType::Ride:
		{
			UnRide();
			break;
		}
		case ECharacterActionType::Climb:
		{
			UnClimb();
			break;
		}
		case ECharacterActionType::Swim:
		{
			UnSwim();
			break;
		}
		default: break;
	}
}

bool ADWCharacter::StopAction(ECharacterActionType InActionType, bool bCancelAbility, bool bEndAction)
{
	if (InActionType == ECharacterActionType::None) InActionType = ActionType;

	if (ActionType == ECharacterActionType::None || ActionType != InActionType) return true;

	if (bCancelAbility)
	{
		const FDWCharacterActionAbilityData AbilityData = GetActionAbility(InActionType);
		if (!AbilityData.bCancelable) return false;

		CancelAbility(AbilityData.AbilityHandle);
	}

	ActionType = ECharacterActionType::None;

	if (bEndAction)
	{
		EndAction(InActionType);
	}
	return true;
}

void ADWCharacter::ModifyEXP(float InDeltaValue)
{
	const int32 MaxEXP = GetMaxEXP();
	EXP += InDeltaValue;
	if (InDeltaValue > 0.f)
	{
		if (EXP >= MaxEXP)
		{
			Level++;
			EXP -= MaxEXP;
		}
	}
	else
	{
		if (EXP < 0.f)
		{
			EXP = 0.f;
		}
	}
	HandleEXPChanged(EXP);
}

void ADWCharacter::ModifyHealth(float InDeltaValue)
{
	if(InDeltaValue < 0.f && GetHealth() > 0.f || InDeltaValue > 0.f && GetHealth() < GetMaxHealth())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetHealthAttribute(), EGameplayModOp::Additive, InDeltaValue);
	}
}

void ADWCharacter::ModifyMana(float InDeltaValue)
{
	if(InDeltaValue < 0.f && GetMana() > 0.f || InDeltaValue > 0.f && GetMana() < GetMaxMana())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetManaAttribute(), EGameplayModOp::Additive, InDeltaValue);
	}
}

void ADWCharacter::ModifyStamina(float InDeltaValue)
{
	if(InDeltaValue < 0.f && GetStamina() > 0.f || InDeltaValue > 0.f && GetStamina() < GetMaxStamina())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetStaminaAttribute(), EGameplayModOp::Additive, InDeltaValue);
	}
}

void ADWCharacter::SetLockedTarget(ADWCharacter* InTargetCharacter)
{
	if(InTargetCharacter)
	{
		LockedTarget = InTargetCharacter;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		LockedTarget = nullptr;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ADWCharacter::LookAtTarget(ADWCharacter* InTargetCharacter)
{
	if(InTargetCharacter && !bDodging)
	{
		const FVector tmpDirection = InTargetCharacter->GetActorLocation() - GetActorLocation();
		SetActorRotation(FRotator(0, tmpDirection.ToOrientationRotator().Yaw, 0));
	}
}

void ADWCharacter::AIMoveTo(FVector InTargetLocation, float InMoveStopDistance /*= 10*/)
{
	AIMoveLocation = InTargetLocation;
	AIMoveStopDistance = InMoveStopDistance;
}

bool ADWCharacter::DoAIMove(FVector InTargetLocation, float InMoveStopDistance /*= 10*/)
{
	if (FVector::Distance(FVector(GetActorLocation().X, GetActorLocation().Y, 0), FVector(InTargetLocation.X, InTargetLocation.Y, 0)) > InMoveStopDistance)
	{
		AddMovementInput(InTargetLocation - GetActorLocation());
		return false;
	}
	return true;
}

bool ADWCharacter::DoAIMove(ADWCharacter* InTargetCharacter, float InMoveStopDistance /*= 10*/, bool bLookAtTarget /*= false*/)
{
	if(!InTargetCharacter || !InTargetCharacter->IsValidLowLevel()) return false;

	if (Distance(InTargetCharacter, false, false) > InMoveStopDistance)
	{
		AddMovementInput(InTargetCharacter->GetActorLocation() - GetActorLocation());
		if (bLookAtTarget) SetLockedTarget(InTargetCharacter);
		return false;
	}
	if (bLookAtTarget) SetLockedTarget(nullptr);
	return true;
}

void ADWCharacter::StopAIMove()
{
	AIMoveLocation = Vector_Empty;
}

void ADWCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (bBreakAllInput) return;

	switch(GetCharacterMovement()->MovementMode)
	{
		case EMovementMode::MOVE_Swimming:
		{
			if(bFloating && WorldDirection.Z > -0.5f)
			{
				WorldDirection.Z = 0;
			}
			break;
		}
		default: break;
	}

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);

	if (!IsPlayerControlled())
	{
		FHitResult hitResult;
		if (RaycastStep(hitResult))
		{
			Jump();
		}
	}
}

void ADWCharacter::SetDamaging(bool bInDamaging)
{
	bDamaging = bInDamaging;
}

UAbilitySystemComponent* ADWCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

bool ADWCharacter::HasBehaviorTree() const
{
	return GetCharacterData().BehaviorTreeAsset != nullptr;
}

UBehaviorTree* ADWCharacter::GetBehaviorTree()
{
	const FCharacterData CharacterData = GetCharacterData();
	if (!BehaviorTree)
	{
		BehaviorTree = DuplicateObject<UBehaviorTree>(CharacterData.BehaviorTreeAsset, this);
		if(BehaviorTree)
		{
			UBlackboardData* Blackboard = DuplicateObject<UBlackboardData>(CharacterData.BehaviorTreeAsset->BlackboardAsset, nullptr);
			BehaviorTree->BlackboardAsset = Blackboard;
		}
	}
	return BehaviorTree;
}

UWidgetCharacterHP* ADWCharacter::GetWidgetCharacterHPWidget() const
{
	if (WidgetCharacterHP->GetUserWidgetObject())
	{
		return Cast<UWidgetCharacterHP>(WidgetCharacterHP->GetUserWidgetObject());
	}
	return nullptr;
}

bool ADWCharacter::IsActive(bool bNeedFreeToAnim /*= false*/) const
{
	return bActive && !IsDead() && (!bNeedFreeToAnim || IsFreeToAnim(false));
}

bool ADWCharacter::IsFreeToAnim(bool bCheckStates) const
{
	return bFreeToAnimate && (!bCheckStates || !bFlying && !bFalling && !bRiding && !bClimbing && !bDefending);
}

bool ADWCharacter::IsDead() const
{
	return bDead || bDying;
}

FCharacterData ADWCharacter::GetCharacterData() const
{
	return UDWHelper::LoadCharacterData(ID);
}

FTeamData* ADWCharacter::GetTeamData() const
{
	return AWorldManager::Get()->GetTeamData(*TeamID);
}

bool ADWCharacter::IsTargetable_Implementation() const
{
	return !IsDead();
}

TArray<EInteractOption> ADWCharacter::GetInteractOptions(IInteraction* InTrigger) const
{
	TArray<EInteractOption> RetValues;
	if(!bDead)
	{
		for(auto Iter : InteractOptions)
		{
			// switch(Iter)
			// {
			// 	default: break;
			// }
		}
	}
	else
	{
		RetValues.Add(EInteractOption::Revive);
	}
	return RetValues;
}

void ADWCharacter::SetNameC(const FString& InName)
{
	Name = InName;
	HandleNameChanged(InName);
}

void ADWCharacter::SetTeamID(const FString& InTeamID)
{
	TeamID = InTeamID;
	HandleTeamIDChanged(InTeamID);
}

void ADWCharacter::SetRaceID(const FString& InRaceID)
{
	RaceID = InRaceID;
	HandleRaceIDChanged(InRaceID);
}

void ADWCharacter::SetLevelC(int32 InLevel)
{
	Level = InLevel;
	HandleLevelChanged(InLevel);
}

void ADWCharacter::SetEXP(int32 InEXP)
{
	EXP = InEXP;
	HandleEXPChanged(InEXP);
}

int32 ADWCharacter::GetMaxEXP() const
{
	int32 MaxEXP = BaseEXP;
	for (int i = 0; i < Level - 1; i++)
	{
		MaxEXP *= EXPFactor;
	}
	return MaxEXP;
}

int32 ADWCharacter::GetTotalEXP() const
{
	int32 TotalEXP = BaseEXP;
	for (int i = 0; i < Level - 1; i++)
	{
		TotalEXP += TotalEXP * EXPFactor;
	}
	return EXP + TotalEXP - GetMaxEXP();
}

FString ADWCharacter::GetHeadInfo() const
{
	return FString::Printf(TEXT("Lv.%d \"%s\" "), Level, *Name);
}

float ADWCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

void ADWCharacter::SetHealth(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetHealthAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

void ADWCharacter::SetMaxHealth(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMana() const
{
	return AttributeSet->GetMana();
}

void ADWCharacter::SetMana(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetManaAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

void ADWCharacter::SetMaxMana(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMaxManaAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetStamina() const
{
	return AttributeSet->GetStamina();
}

void ADWCharacter::SetStamina(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMaxStamina() const
{
	return AttributeSet->GetMaxStamina();
}

void ADWCharacter::SetMaxStamina(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

void ADWCharacter::SetMoveSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMoveSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetSwimSpeed() const
{
	return AttributeSet->GetSwimSpeed();
}

void ADWCharacter::SetSwimSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetSwimSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetRideSpeed() const
{
	return AttributeSet->GetRideSpeed();
}

void ADWCharacter::SetRideSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetRideSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetFlySpeed() const
{
	return AttributeSet->GetFlySpeed();
}

void ADWCharacter::SetFlySpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetFlySpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetRotationSpeed() const
{
	return AttributeSet->GetRotationSpeed();
}

void ADWCharacter::SetRotationSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetRotationSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetJumpForce() const
{
	return AttributeSet->GetJumpForce();
}

void ADWCharacter::SetJumpForce(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetJumpForceAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetDodgeForce() const
{
	return AttributeSet->GetDodgeForce();
}

void ADWCharacter::SetDodgeForce(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetDodgeForceAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetAttackForce() const
{
	return AttributeSet->GetAttackForce();
}

void ADWCharacter::SetAttackForce(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetAttackForceAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetRepulseForce() const
{
	return AttributeSet->GetRepulseForce();
}

void ADWCharacter::SetRepulseForce(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetRepulseForceAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetAttackSpeed() const
{
	return AttributeSet->GetAttackSpeed();
}

void ADWCharacter::SetAttackSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetAttackSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetAttackCritRate() const
{
	return AttributeSet->GetAttackCritRate();
}

void ADWCharacter::SetAttackCritRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetAttackCritRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetAttackStealRate() const
{
	return AttributeSet->GetAttackStealRate();
}

void ADWCharacter::SetAttackStealRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetAttackStealRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetDefendRate() const
{
	return AttributeSet->GetDefendRate();
}

void ADWCharacter::SetDefendRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetDefendRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetDefendScope() const
{
	return AttributeSet->GetDefendScope();
}

void ADWCharacter::SetDefendScope(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetDefendScopeAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetPhysicsDefRate() const
{
	return AttributeSet->GetPhysicsDefRate();
}

void ADWCharacter::SetPhysicsDefRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetPhysicsDefRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetMagicDefRate() const
{
	return AttributeSet->GetMagicDefRate();
}

void ADWCharacter::SetMagicDefRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMagicDefRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetToughnessRate() const
{
	return AttributeSet->GetToughnessRate();
}

void ADWCharacter::SetToughnessRate(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetToughnessRateAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetStaminaRegenSpeed() const
{
	return AttributeSet->GetStaminaRegenSpeed();
}

void ADWCharacter::SetStaminaRegenSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetStaminaRegenSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetStaminaExpendSpeed() const
{
	return AttributeSet->GetStaminaExpendSpeed();
}

void ADWCharacter::SetStaminaExpendSpeed(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetStaminaExpendSpeedAttribute(), EGameplayModOp::Override, InValue);
}

float ADWCharacter::GetPhysicsDamage() const
{
	return AttributeSet->GetPhysicsDamage();
}

float ADWCharacter::GetMagicDamage() const
{
	return AttributeSet->GetMagicDamage();
}

UInventory* ADWCharacter::GetInventory() const
{
	return Inventory;
}

FVector ADWCharacter::GetMoveVelocity(bool bIgnoreZ) const
{
	FVector Velocity = GetMovementComponent()->Velocity;
	if(bIgnoreZ) Velocity.Z = 0;
	return Velocity;
}

FVector ADWCharacter::GetMoveDirection(bool bIgnoreZ) const
{
	return GetMoveVelocity(bIgnoreZ).GetSafeNormal();
}

float ADWCharacter::GetRadius() const
{
	if(!GetCapsuleComponent()) return 0.f;

	return GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float ADWCharacter::GetHalfHeight() const
{
	if(!GetCapsuleComponent()) return 0.f;

	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

bool ADWCharacter::HasWeapon(EWeaponType InWeaponType)
{
	if(InWeaponType == EWeaponType::None) return true;
	
	if(HasEquip(EEquipPartType::RightHand))
	{
		if(AEquipWeapon* Weapon = Cast<AEquipWeapon>(GetEquip(EEquipPartType::RightHand)))
		{
			return Weapon->GetWeaponData().WeaponType == InWeaponType;
		}
	}
	return false;
}

bool ADWCharacter::HasShield(EShieldType InShieldType)
{
	if(InShieldType == EShieldType::None) return true;
	
	if(HasEquip(EEquipPartType::LeftHand))
	{
		if(AEquipShield* Weapon = Cast<AEquipShield>(GetEquip(EEquipPartType::LeftHand)))
		{
			return Weapon->GetShieldData().ShieldType == InShieldType;
		}
	}
	return false;
}

AEquipWeapon* ADWCharacter::GetWeapon()
{
	return Cast<AEquipWeapon>(GetEquip(EEquipPartType::RightHand));
}

AEquipShield* ADWCharacter::GetShield()
{
	return Cast<AEquipShield>(GetEquip(EEquipPartType::LeftHand));
}

bool ADWCharacter::HasArmor(EEquipPartType InPartType)
{
	return HasEquip(InPartType) && GetEquip(InPartType)->IsA(AEquipArmor::StaticClass());
}

AEquipArmor* ADWCharacter::GetArmor(EEquipPartType InPartType)
{
	if (HasArmor(InPartType))
	{
		return Cast<AEquipArmor>(GetEquip(InPartType));
	}
	return nullptr;
}

bool ADWCharacter::HasEquip(EEquipPartType InPartType)
{
	return Equips.Contains(InPartType) && Equips[InPartType];
}

AEquip* ADWCharacter::GetEquip(EEquipPartType InPartType)
{
	if (HasEquip(InPartType))
	{
		return Equips[InPartType];
	}
	return nullptr;
}

FDWCharacterAttackAbilityData ADWCharacter::GetAttackAbility(int32 InAbilityIndex /*= -1*/)
{
	if (HasAttackAbility(InAbilityIndex == -1 ? AttackAbilityIndex : InAbilityIndex))
	{
		return AttackAbilities[InAbilityIndex == -1 ? AttackAbilityIndex : InAbilityIndex];
	}
	return FDWCharacterAttackAbilityData();
}

FDWCharacterSkillAbilityData ADWCharacter::GetSkillAbility(const FName& InSkillID)
{
	if(HasSkillAbility(InSkillID))
	{
		return SkillAbilities[InSkillID];
	}
	return FDWCharacterSkillAbilityData();
}

FDWCharacterSkillAbilityData ADWCharacter::GetSkillAbility(ESkillType InSkillType, int32 InAbilityIndex)
{
	if(HasSkillAbility(InSkillType, InAbilityIndex))
	{
		TArray<FDWCharacterSkillAbilityData> Abilities = TArray<FDWCharacterSkillAbilityData>();
		for (auto Iter : SkillAbilities)
		{
			if(Iter.Value.GetItemData().SkillType == InSkillType)
			{
				Abilities.Add(Iter.Value);
			}
		}
		return InAbilityIndex != -1 ? Abilities[InAbilityIndex] : Abilities[0];
	}
	return FDWCharacterSkillAbilityData();
}

FDWCharacterActionAbilityData ADWCharacter::GetActionAbility(ECharacterActionType InActionType /*= ECharacterActionType::None*/)
{
	if(InActionType == ECharacterActionType::None) InActionType = ActionType;
	if (HasActionAbility(InActionType))
	{
		return ActionAbilities[InActionType];
	}
	return FDWCharacterActionAbilityData();
}

bool ADWCharacter::RaycastStep(FHitResult& OutHitResult)
{
	const FVector rayStart = GetActorLocation() + FVector::DownVector * (GetHalfHeight() - GetCharacterMovement()->MaxStepHeight);
	const FVector rayEnd = rayStart + GetMoveDirection() * (GetRadius() + AWorldManager::GetWorldData().BlockSize * FMath::Clamp(GetMoveDirection().Size() * 0.005f, 0.5f, 1.3f));
	return UKismetSystemLibrary::LineTraceSingle(this, rayStart, rayEnd, UDWHelper::GetGameTrace(EGameTraceType::Step), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
}

bool ADWCharacter::RaycastVoxel(FVoxelHitResult& OutHitResult)
{
	FHitResult hitResult;
	const FVector rayStart = GetActorLocation();
	const FVector rayEnd = rayStart + GetActorForwardVector() * InteractDistance;
	if (UKismetSystemLibrary::LineTraceSingle(this, rayStart, rayEnd, UDWHelper::GetGameTrace(EGameTraceType::Voxel), false, TArray<AActor*>(), EDrawDebugTrace::None, hitResult, true))
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
	return false;
}

bool ADWCharacter::HasTeam() const
{
	return GetTeamData()->IsValid();
}

bool ADWCharacter::IsTeamMate(ADWCharacter* InTargetCharacter) const
{
	return HasTeam() && InTargetCharacter->TeamID == TeamID;
}

bool ADWCharacter::HasAttackAbility(int32 InAbilityIndex /*= -1*/) const
{
	if (InAbilityIndex != -1)
	{
		return AttackAbilities.Num() > InAbilityIndex;
	}
	else
	{
		return AttackAbilities.Num() > 0;
	}
}

bool ADWCharacter::HasSkillAbility(const FName& InSkillID)
{
	if(SkillAbilities.Contains(InSkillID))
	{
		auto SkillSlots = Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill);
		for (int32 i = 0; i < SkillSlots.Num(); i++)
		{
			if(SkillSlots[i]->GetItem().ID == InSkillID)
			{
				return true;
			}
		}
	}
	return false;
}

bool ADWCharacter::HasSkillAbility(ESkillType InSkillType, int32 InAbilityIndex)
{
	TArray<FDWCharacterSkillAbilityData> Abilities = TArray<FDWCharacterSkillAbilityData>();
	for (auto Iter : SkillAbilities)
	{
		if(Iter.Value.GetItemData().SkillType == InSkillType)
		{
			Abilities.Add(Iter.Value);
		}
	}
	if(InAbilityIndex != -1 ? Abilities.IsValidIndex(InAbilityIndex) : Abilities.Num() > 0)
	{
		auto SkillSlots = Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill);
		for (int32 i = 0; i < SkillSlots.Num(); i++)
		{
			if(SkillSlots[i]->GetItem().ID == Abilities[InAbilityIndex != -1 ? InAbilityIndex : 0].AbilityName)
			{
				return true;
			}
		}
	}
	return false;
}

bool ADWCharacter::HasActionAbility(ECharacterActionType InActionType)
{
	if (ActionAbilities.Contains(InActionType))
	{
		return true;
	}
	return false;
}

bool ADWCharacter::CreateTeam(const FName& InTeamName /*= MANE_None*/, FString InTeamDetail /*= TEXT("")*/)
{
	return AWorldManager::Get()->CreateTeam(this, InTeamName, InTeamDetail);
}

bool ADWCharacter::DissolveTeam()
{
	return AWorldManager::Get()->DissolveTeam(*TeamID, this);
}

bool ADWCharacter::JoinTeam(const FName& InTeamID)
{
	if (AWorldManager::Get()->IsExistTeam(InTeamID))
	{
		AWorldManager::Get()->GetTeamData(InTeamID)->AddMember(this);
		return true;
	}
	return false;
}

bool ADWCharacter::JoinTeam(ADWCharacter* InTargetCharacter)
{
	return JoinTeam(*InTargetCharacter->GetTeamID());
}

bool ADWCharacter::LeaveTeam()
{
	if (HasTeam())
	{
		GetTeamData()->RemoveMember(this);
		return true;
	}
	return false;
}

bool ADWCharacter::AddTeamMate(ADWCharacter* InTargetCharacter)
{
	if (HasTeam() && GetTeamData()->IsCaptain(this))
	{
		GetTeamData()->AddMember(InTargetCharacter);
		return true;
	}
	return false;
}

bool ADWCharacter::RemoveTeamMate(ADWCharacter* InTargetCharacter)
{
	if (HasTeam() && GetTeamData()->IsCaptain(this))
	{
		GetTeamData()->RemoveMember(InTargetCharacter);
		return true;
	}
	return false;
}

TArray<ADWCharacter*> ADWCharacter::GetTeamMates()
{
	return GetTeamData()->GetMembers(this);
}

void ADWCharacter::SpawnWidgetWorldText(EWorldTextType InContextType, FString InContext)
{
	auto contextHUD = NewObject<UWidgetWorldTextComponent>(this);
	contextHUD->RegisterComponent();
	contextHUD->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	contextHUD->SetRelativeLocation(FVector(0, 0, 0));
	contextHUD->SetVisibility(false);
	if (contextHUD && contextHUD->GetUserWidgetObject())
	{
		auto tmpWidget = Cast<UWidgetWorldText>(contextHUD->GetUserWidgetObject());
		if (tmpWidget)
		{
			tmpWidget->InitWidgetWorldText(InContextType, InContext);
		}
	}
}

bool ADWCharacter::IsPlayer() const
{
	return Nature == ECharacterNature::Player;
}

bool ADWCharacter::IsEnemy(ADWCharacter* InTargetCharacter) const
{
	switch (Nature)
	{
		case ECharacterNature::NPC:
		case ECharacterNature::AIFriendly:
		{
			return false;
		}
		default:
		{
			switch (InTargetCharacter->Nature)
			{
				case ECharacterNature::NPC:
				case ECharacterNature::AIFriendly:
				{
					return false;
				}
				default: break;
			}
			break;
		}
	}
	return !IsTeamMate(InTargetCharacter) && !InTargetCharacter->GetRaceID().Equals(RaceID);
}

float ADWCharacter::Distance(ADWCharacter* InTargetCharacter, bool bIgnoreRadius /*= true*/, bool bIgnoreZAxis /*= true*/)
{
	if(!InTargetCharacter || !InTargetCharacter->IsValidLowLevel()) return -1;
	return FVector::Distance(FVector(GetActorLocation().X, GetActorLocation().Y, bIgnoreZAxis ? 0 : GetActorLocation().Z), FVector(InTargetCharacter->GetActorLocation().X, InTargetCharacter->GetActorLocation().Y, bIgnoreZAxis ? 0 : InTargetCharacter->GetActorLocation().Z)) - (bIgnoreRadius ? 0 : InTargetCharacter->GetRadius());
}

void ADWCharacter::SetVisible_Implementation(bool bVisible)
{
	GetRootComponent()->SetVisibility(bVisible, true);
}

void ADWCharacter::SetMotionRate(float InMovementRate, float InRotationRate)
{
	MovementRate = InMovementRate;
	RotationRate = InRotationRate;
	HandleMoveSpeedChanged(GetMoveSpeed());
	HandleRotationSpeedChanged(GetRotationSpeed());
	HandleSwimSpeedChanged(GetSwimSpeed());
	HandleRideSpeedChanged(GetRideSpeed());
	HandleFlySpeedChanged(GetFlySpeed());
}

UDWCharacterPart* ADWCharacter::GetCharacterPart(ECharacterPartType InCharacterPartType) const
{
	TArray<UDWCharacterPart*> Parts;
	GetComponents(Parts);
	for(auto Iter : Parts)
	{
		if(Iter->GetCharacterPartType() == InCharacterPartType)
		{
			return Iter;
		}
	}
	return nullptr;
}

void ADWCharacter::HandleDamage(const float LocalDamageDone, FHitResult HitResult, const struct FGameplayTagContainer& SourceTags, ADWCharacter* SourceCharacter, AActor* SourceActor)
{
	if (GetHealth() <= 0.f)
	{
		Death(SourceCharacter);
	}
	if(SourceCharacter && SourceCharacter != this)
	{
		SourceCharacter->ModifyHealth(LocalDamageDone * SourceCharacter->GetAttackStealRate());
		if(!IsDead() && !IsPlayer())
		{
			if(!GetController<ADWAIController>()->GetTargetCharacter())
			{
				GetController<ADWAIController>()->SetTargetCharacter(SourceCharacter);
			}
		}
	}
}

void ADWCharacter::HandleInterrupt(float InterruptDuration)
{
	if (!FMath::IsNearlyZero(InterruptDuration))
	{
		Interrupt(InterruptDuration * (1 - GetToughnessRate()), true);
	}
}

void ADWCharacter::HandleNameChanged(const FString& NewValue)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void ADWCharacter::HandleTeamIDChanged(const FString& NewValue)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void ADWCharacter::HandleRaceIDChanged(const FString& NewValue)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void ADWCharacter::HandleLevelChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	DefaultAbility.AbilityLevel = NewValue;
	FallingAttackAbility.AbilityLevel = NewValue;
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void ADWCharacter::HandleEXPChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void ADWCharacter::HandleHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (!FMath::IsNearlyZero(DeltaValue))
	{
		SpawnWidgetWorldText(DeltaValue > 0.f ? EWorldTextType::Recover : (!IsPlayer() ? EWorldTextType::DamagePlayer : EWorldTextType::DamageOther), FString::FromInt(FMath::Abs(DeltaValue)));
	}
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHealthPercent(NewValue, GetMaxHealth());
	}
}

void ADWCharacter::HandleMaxHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetHealthPercent(NewValue, GetMaxHealth());
	}
}

void ADWCharacter::HandleManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetManaPercent(NewValue, GetMaxMana());
	}
}

void ADWCharacter::HandleMaxManaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetManaPercent(NewValue, GetMaxMana());
	}
}

void ADWCharacter::HandleStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetStaminaPercent(NewValue, GetMaxStamina());
	}
}

void ADWCharacter::HandleMaxStaminaChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetCharacterHPWidget())
	{
		GetWidgetCharacterHPWidget()->SetStaminaPercent(NewValue, GetMaxStamina());
	}
}

void ADWCharacter::HandleMoveSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->MaxWalkSpeed = NewValue * (bSprinting ? 1.5f : 1) * MovementRate;
}

void ADWCharacter::HandleSwimSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->MaxSwimSpeed = NewValue * (bSprinting ? 1.5f : 1) * MovementRate;
}

void ADWCharacter::HandleRideSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->MaxWalkSpeed = NewValue * (bSprinting ? 1.5f : 1) * MovementRate;
}

void ADWCharacter::HandleFlySpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->MaxFlySpeed = NewValue * (bSprinting ? 1.5f : 1) * MovementRate;
}

void ADWCharacter::HandleRotationSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->RotationRate = FRotator(0, NewValue * (bSprinting ? 1.5f : 1), 0) * RotationRate;
}

void ADWCharacter::HandleJumpForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	GetCharacterMovement()->JumpZVelocity = NewValue;
}

void ADWCharacter::HandleDodgeForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleAttackForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleRepulseForceChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleAttackSpeedChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleAttackCritRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleAttackStealRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleDefendRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleDefendScopeChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandlePhysicsDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleMagicDefRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleToughnessRateChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	
}

void ADWCharacter::HandleRegenSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{

}

void ADWCharacter::HandleExpendSpeedAttribute(float NewValue, float DeltaValue /*= 0.f*/)
{

}
