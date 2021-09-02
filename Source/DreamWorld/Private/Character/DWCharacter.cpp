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
#include "Widget/Worlds/WidgetCharacterHP.h"
#include "Widget/Worlds/WidgetWorldText.h"
#include "Inventory/Character/CharacterInventory.h"
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
#include "AI/DWAIBlackboard.h"
#include "GameFramework/PhysicsVolume.h"
#include "Voxel/Voxel.h"
#include "Abilities/Character/DWCharacterActionAbility.h"
#include "Abilities/Character/DWCharacterAttackAbility.h"
#include "Abilities/Character/DWCharacterSkillAbility.h"

//////////////////////////////////////////////////////////////////////////
// ADWCharacter

ADWCharacter::ADWCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Damage::StaticClass());

	WidgetCharacterHP = CreateDefaultSubobject<UWidgetCharacterHPComponent>(TEXT("WidgetCharacterHP"));
	WidgetCharacterHP->SetupAttachment(RootComponent);
	WidgetCharacterHP->SetRelativeLocation(FVector(0, 0, 70));
	WidgetCharacterHP->SetVisibility(false);

	AbilitySystem = CreateDefaultSubobject<UDWAbilitySystemComponent>(TEXT("AbilitySystem"));

	AttributeSet = CreateDefaultSubobject<UDWCharacterAttributeSet>(TEXT("AttributeSet"));

	AnimInstance = nullptr;
	BehaviorTree = nullptr;
	Blackboard = nullptr;

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

	// states
	bDead = true;
	bDying = false;
	bActive = false;

	bFalling = false;
	bDodging = false;
	bSprinting = false;
	bCrouching = false;
	bSwimming = false;
	bClimbing = false;
	bRiding = false;
	bFlying = false;
	bAttacking = false;
	bDefending = false;
	bDamaging = false;
	bBlocking = false;
	bInterrupting = false;
	bLockRotation = false;
	bFreeToAnimate = true;
	bBreakAllInput = false;

	// stats
	Name = TEXT("");
	Nature = ECharacterNature::AIHostile;
	TeamID = TEXT("");
	RaceID = TEXT("");
	Level = 0;
	EXP = 0;
	BaseEXP = 100;
	EXPFactor = 2.f;
	AttackDistance = 100.f;
	FollowDistance = 500.f;
	PatrolDistance = 1000.f;
	PatrolDuration = 10.f;
	bAutoJump = true;
	
	Equips = TMap<EEquipPartType, AEquip*>();
	for (int32 i = 0; i < 6; i++)
	{
		Equips.Add((EEquipPartType)i, nullptr);
	}

	MovementRate = 1;
	RotationRate = 1;
	MoveVelocity = FVector(0);
	MoveDirection = FVector(0);
	OwnerChunk = nullptr;
	OverlapVoxel = nullptr;
	Inventory = nullptr;

	// setup

	PassiveEffectTable = nullptr;
	AttackAbilityTable = nullptr;
	SkillAbilityTable = nullptr;
	ActionAbilityTable = nullptr;
	BehaviorTreeAsset = nullptr;
	BlackboardAsset = nullptr;

	// local
	AttackAbilityIndex = 0;
	SkillAbilityIndex = NAME_None;
	AttackType = EAttackType::None;
	ActionType = ECharacterActionType::None;
	LockedTarget = nullptr;
	BirthLocation = FVector(0, 0, 0);
	AIMoveLocation = Vector_Empty;
	AIMoveStopDistance = 0;
	DodgeRemainTime = 0;
	InterruptRemainTime = 0;

	PassiveEffects = TArray<FDWCharacterPassiveEffectData>();
	AttackAbilitys = TArray<FDWCharacterAttackAbilityData>();
	SkillAbilitys = TMap<FName, FDWCharacterSkillAbilityData>();
	ActionAbilitys = TMap<ECharacterActionType, FDWCharacterActionAbilityData>();

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ADWAIController::StaticClass();
}

// Called when the game starts or when spawned
void ADWCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystem->InitAbilityActorInfo(this, this);

	AnimInstance = Cast<UDWCharacterAnim>(GetMesh()->GetAnimInstance());
	BirthLocation = GetActorLocation();

	FString contextString;

	if (AttackAbilityTable != nullptr)
	{
		TArray<FDWCharacterAttackAbilityData*> attackAbilitys;
		AttackAbilityTable->GetAllRows(contextString, attackAbilitys);
		for (int i = 0; i < attackAbilitys.Num(); i++)
		{
			auto abilityData = *attackAbilitys[i];
			abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
			AttackAbilitys.Add(abilityData);
		}
	}

	if (SkillAbilityTable != nullptr)
	{
		TArray<FDWCharacterSkillAbilityData*> skillAbilitys;
		SkillAbilityTable->GetAllRows(contextString, skillAbilitys);
		for (int i = 0; i < skillAbilitys.Num(); i++)
		{
			auto abilityData = *skillAbilitys[i];
			abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
			SkillAbilitys.Add(abilityData.AbilityName, abilityData);
		}
	}
	
	if (FallingAttackAbility.AbilityClass != nullptr)
	{
		FallingAttackAbility.AbilityHandle = AcquireAbility(FallingAttackAbility.AbilityClass, FallingAttackAbility.AbilityLevel);
	}

	if (ActionAbilityTable != nullptr)
	{
		TArray<FDWCharacterActionAbilityData*> actionAbilitys;
		ActionAbilityTable->GetAllRows(contextString, actionAbilitys);
		for (int i = 0; i < actionAbilitys.Num(); i++)
		{
			auto abilityData = *actionAbilitys[i];
			abilityData.AbilityHandle = AcquireAbility(abilityData.AbilityClass, abilityData.AbilityLevel);
			ActionAbilitys.Add(abilityData.ActionType, abilityData);
		}
	}

	if (PassiveEffectTable != nullptr)
	{
		TArray<FDWCharacterPassiveEffectData*> passiveEffects;
		PassiveEffectTable->GetAllRows(contextString, passiveEffects);
		for (int i = 0; i < passiveEffects.Num(); i++)
		{
			auto effectData = *passiveEffects[i];
			effectData.EffectHandle = ApplyEffect(effectData.EffectClass);
			PassiveEffects.Add(effectData);
		}
	}

	if (GetWidgetCharacterHPWidget() && !GetWidgetCharacterHPWidget()->GetOwnerCharacter())
	{
		GetWidgetCharacterHPWidget()->SetOwnerCharacter(this);
	}

	Revive();
}

// Called every frame
void ADWCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

	if (bActive)
	{
		if (InterruptRemainTime != -1)
		{
			InterruptRemainTime -= DeltaTime;
			if (InterruptRemainTime <= 0)
			{
				UnInterrupt();
			}
		}

		if (bDodging && DodgeRemainTime > 0)
		{
			DodgeRemainTime -= DeltaTime;
			if (DodgeRemainTime <= 0)
			{
				UnDodge();
			}
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

		MoveVelocity = GetCharacterMovement()->Velocity;
		MoveVelocity.Z = 0;
		if (MoveVelocity.Size() > 0.2f)
		{
			MoveDirection = MoveVelocity.GetSafeNormal();
		}
		else
		{
			MoveDirection = GetActorForwardVector();
		}

		FVector Location = GetMesh()->GetSocketLocation(FName("Foot"));
		
		AChunk* Chunk = AWorldManager::GetCurrent()->FindChunk(Location);
		if(Chunk != nullptr)
		{
			if(OwnerChunk != nullptr)
			{
				OwnerChunk->DetachCharacter(this);
			}
			Chunk->AttachCharacter(this);
		}
		else if(OwnerChunk != nullptr)
		{
			OwnerChunk->DetachCharacter(this);
		}

		if(OwnerChunk != nullptr)
		{
			UVoxel* Voxel = OwnerChunk->GetVoxel(OwnerChunk->LocationToIndex(Location));
			if(Voxel != nullptr)
			{
				if(OverlapVoxel != Voxel)
				{
					if(OverlapVoxel != nullptr)
					{
						OverlapVoxel->OnTargetExit(this, FVoxelHitResult(OverlapVoxel, Location, MoveDirection));
					}
					Voxel->OnTargetEnter(this, FVoxelHitResult(Voxel, Location, MoveDirection));
				}
				Voxel->OnTargetStay(this, FVoxelHitResult(Voxel, Location, MoveDirection));
			}
			else if(OverlapVoxel != nullptr)
			{
				OverlapVoxel->OnTargetExit(this, FVoxelHitResult(OverlapVoxel, Location, MoveDirection));
			}
		}

		if (bSprinting && MoveVelocity.Size() > 0.2f)
		{
			ModifyStamina(-GetStaminaExpendSpeed() * DeltaTime);
		}
		else if(bFreeToAnimate)
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
				OnFallEnd();
			}
			break;
		}
		case EMovementMode::MOVE_Falling:
		{
			if (!bFalling)
			{
				OnFallStart();
			}
			if (GetActorLocation().Z < 0)
			{
				Death();
			}
			break;
		}
	}

	if (bDying && (!bFalling || GetActorLocation().Z < 0) && ActionType != ECharacterActionType::Death)
	{
		OnDeathStart();
	}
}

void ADWCharacter::LoadData(FCharacterData InSaveData)
{
	if (!Inventory) Inventory = NewObject<UCharacterInventory>(this);

	if (InSaveData.bInitialized)
	{
		SetName(InSaveData.Name);
		SetRaceID(InSaveData.RaceID);
		SetTeamID(InSaveData.TeamID);
		SetLevelC(InSaveData.Level);
		SetEXP(InSaveData.EXP);

		//AttributeSet = InSaveData.AttributeSet;
		Inventory->LoadData(InSaveData.InventoryData, this);

		SetActorLocation(InSaveData.Location);
		SetActorRotation(InSaveData.Rotation);
	}
	else
	{
		SetRaceID(InSaveData.RaceID);

		Inventory->LoadData(InventoryData, this);

		if(Nature != ECharacterNature::Player)
		{
			const auto ItemDatas = UDWHelper::LoadItemDatas();
			if(FMath::FRandRange(0, 1) < 0.5f)
			{
				InventoryData.Items.Add(FItem(ItemDatas[FMath::RandRange(0, ItemDatas.Num() - 1)].ID));
			}
		}

		SetActorLocation(InSaveData.Location);
		SetActorRotation(InSaveData.Rotation);
	}
}

FCharacterData ADWCharacter::ToData()
{
	auto SaveData = FCharacterData();

	SaveData.bInitialized = true;

	SaveData.Name = Name;
	SaveData.TeamID = TeamID;
	SaveData.RaceID = RaceID;
	SaveData.Level = Level;
	SaveData.EXP = EXP;

	SaveData.AttributeSet = AttributeSet;
	SaveData.InventoryData = Inventory->ToData();

	SaveData.Location = GetActorLocation();
	SaveData.Rotation = GetActorRotation();

	SaveData.Class = GetClass();

	return SaveData;
}

bool ADWCharacter::IsActive(bool bNeedFreeToAnim /*= false*/) const
{
	return bActive && !IsDead() && (!bNeedFreeToAnim || bFreeToAnimate);
}

bool ADWCharacter::IsDead() const
{
	return bDead || bDying;
}

bool ADWCharacter::HasTeam() const
{
	return GetTeamData()->IsValid();
}

bool ADWCharacter::IsTeamMate(ADWCharacter* InTargetCharacter) const
{
	return HasTeam() && InTargetCharacter->TeamID == TeamID;
}

bool ADWCharacter::HasAttackAbility(int32 InAbilityIndex /*= -1*/)
{
	if (InAbilityIndex != -1)
	{
		return AttackAbilitys.Num() > InAbilityIndex;
	}
	else
	{
		return AttackAbilitys.Num() > 0;
	}
}

bool ADWCharacter::HasSkillAbility(const FName& InSkillID)
{
	if(SkillAbilitys.Contains(InSkillID))
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
	TArray<FDWCharacterSkillAbilityData> Abilitys = TArray<FDWCharacterSkillAbilityData>();
	for (auto Iter : SkillAbilitys)
	{
		if(Iter.Value.GetItemData().SkillType == InSkillType)
		{
			Abilitys.Add(Iter.Value);
		}
	}
	if(InAbilityIndex != -1 ? Abilitys.IsValidIndex(InAbilityIndex) : Abilitys.Num() > 0)
	{
		auto SkillSlots = Inventory->GetSplitSlots<UInventorySkillSlot>(ESplitSlotType::Skill);
		for (int32 i = 0; i < SkillSlots.Num(); i++)
		{
			if(SkillSlots[i]->GetItem().ID == Abilitys[InAbilityIndex != -1 ? InAbilityIndex : 0].AbilityName)
			{
				return true;
			}
		}
	}
	return false;
}

bool ADWCharacter::HasActionAbility(ECharacterActionType InActionType)
{
	if (ActionAbilitys.Contains(InActionType))
	{
		return true;
	}
	return false;
}

bool ADWCharacter::CreateTeam(const FName& InTeamName /*= MANE_None*/, FString InTeamDetail /*= TEXT("")*/)
{
	return AWorldManager::GetCurrent()->CreateTeam(this, InTeamName, InTeamDetail);
}

bool ADWCharacter::DissolveTeam()
{
	return AWorldManager::GetCurrent()->DissolveTeam(*TeamID, this);
}

bool ADWCharacter::JoinTeam(const FName& InTeamID)
{
	if (AWorldManager::GetCurrent()->IsExistTeam(InTeamID))
	{
		AWorldManager::GetCurrent()->GetTeamData(InTeamID)->AddMember(this);
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

bool ADWCharacter::ValidInteract(ADWCharacter* InTargetCharacter, ECharacterInteract InCharacterInteract)
{
	if (!InTargetCharacter || !InTargetCharacter->IsValidLowLevel()) return false;

	switch (InCharacterInteract)
	{
		case ECharacterInteract::Fighting:
		case ECharacterInteract::Damage:
		{
			switch (InTargetCharacter->Nature)
			{
				case ECharacterNature::NPC:
				case ECharacterNature::AIFriendly:
					return false;
				default:
					return !IsTeamMate(InTargetCharacter) && !InTargetCharacter->GetRaceID().Equals(RaceID);
			}
		}
		case ECharacterInteract::Dialogue:
		case ECharacterInteract::Transaction:
		{
			switch (InTargetCharacter->Nature)
			{
				case ECharacterNature::NPC:
				case ECharacterNature::AIFriendly:
					return true;
				default:
					return IsTeamMate(InTargetCharacter);
			}
		}
	}
	return false;
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

void ADWCharacter::Active(bool bResetStats /*= false*/)
{
	if (!bActive)
	{
		bActive = true;
		UnInterrupt();
		GetCharacterMovement()->SetActive(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WidgetCharacterHP->SetVisibility(true);
	}
	if (bResetStats)
	{
		// states
		bDead = false;
		bDying = false;
		bDodging = false;
		bSprinting = false;
		bCrouching = false;
		bSwimming = false;
		bAttacking = false;
		bDefending = false;
		bDamaging = false;
		bBlocking = false;

		// stats
		SetMotionRate(1, 1);
		SetLockedTarget(nullptr);
			
		// local
		AttackAbilityIndex = 0;
		AIMoveLocation = Vector_Empty;
		AIMoveStopDistance = 0;
		DodgeRemainTime = 0;
		InterruptRemainTime = 0;
		ActionType = ECharacterActionType::None;
	}
}

void ADWCharacter::Disable(bool bDisableMovement, bool bDisableCollision)
{
	if (bActive)
	{
		bActive = false;
		Interrupt();
		WidgetCharacterHP->SetVisibility(false);
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
		if (Nature != ECharacterNature::Player)
		{
			GetTeamData()->RemoveMember(this);
			GetController<ADWAIController>()->UnPossess();
		}
	}
}

void ADWCharacter::Revive()
{
	if (bDead && !bDying)
	{
		bDead = false;
		bDying = false;
		Active(true);
		SetVisible(true);
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
		DoAction(ECharacterActionType::Revive);
	}
}

void ADWCharacter::Refresh()
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

void ADWCharacter::FreeToAnim(bool bResetStats /*= false*/)
{
	if (!bFreeToAnimate && !bInterrupting && !bFalling)
	{
		bFreeToAnimate = true;
		bLockRotation = false;
	}
	if (bResetStats)
	{
		SetMotionRate(1, 1);
	}
}

void ADWCharacter::LimitToAnim(bool bInLockRotation /*= false*/, bool bUnSprint /*= false*/)
{
	if (bFreeToAnimate)
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
		UnSprint();
		UnJump();
		UnDodge();
		UnCrouch();
		UnSwim();
		UnAttack();
		UnDefend();
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
		FreeToAnim(true);
		StopAction(ECharacterActionType::Interrupt);
	}
}

void ADWCharacter::Jump()
{
	if (bFreeToAnimate && !bSwimming)
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
	if (bFreeToAnimate && MoveVelocity.Size2D() > 0)
	{
		if (DoAction(ECharacterActionType::Dodge))
		{
			bDodging = true;
			LimitToAnim(true);
			GetCapsuleComponent()->SetGenerateOverlapEvents(false);
			SetActorRotation(FRotator(0.f, MoveDirection.ToOrientationRotator().Yaw, 0.f));
		}
	}
}

void ADWCharacter::UnDodge()
{
	if (bDodging)
	{
		bDodging = false;
		FreeToAnim();
		StopAction(ECharacterActionType::Dodge);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	}
}

void ADWCharacter::Sprint()
{
	if (bFreeToAnimate)
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
	if (!bCrouching /*&& DoAction(ECharacterActionType::Crouch)*/)
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
	if(!bSwimming /*&& DoAction(ECharacterActionType::Swim)*/)
	{
		bSwimming = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
		GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = true;
		//FVector Velocity = GetMovementComponent()->Velocity;
		//GetMovementComponent()->Velocity = FVector(Velocity.X, Velocity.Y, 0);
	}
}

void ADWCharacter::UnSwim()
{
	if (bSwimming)
	{
		bSwimming = false;
		//StopAction(ECharacterActionType::Swim);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = false;
		//FVector Velocity = GetMovementComponent()->Velocity;
		//GetMovementComponent()->Velocity = FVector(Velocity.X, Velocity.Y, 0);
	}
}

void ADWCharacter::Climb()
{
	if (!bClimbing /*&& DoAction(ECharacterActionType::Climb)*/)
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
		//StopAction(ECharacterActionType::Climb);
	}
}

void ADWCharacter::Ride()
{
	if (!bRiding /*&& DoAction(ECharacterActionType::Ride)*/)
	{
		bRiding = true;
		LimitToAnim();
	}
}

void ADWCharacter::UnRide()
{
	if (bRiding)
	{
		bRiding = false;
		FreeToAnim();
		//StopAction(ECharacterActionType::Ride);
	}
}

void ADWCharacter::Fly()
{
	if (bFalling /*&& DoAction(ECharacterActionType::Fly)*/)
	{
		bFalling = true;
		LimitToAnim();
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void ADWCharacter::UnFly()
{
	if (bFalling)
	{
		bFalling = false;
		FreeToAnim();
		//StopAction(ECharacterActionType::Fly);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool ADWCharacter::Attack(int32 InAbilityIndex /*= -1*/)
{
	if (bFreeToAnimate)
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
	if (bFreeToAnimate)
	{
		if (HasSkillAbility(InSkillID))
		{
			auto AbilityData = GetSkillAbility(InSkillID);
			switch(AbilityData.GetItemData().SkillMode)
			{
				case ESkillMode::Initiative:
				{
					if(AbilityData.GetItemData().SkillMode == ESkillMode::Initiative)
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
	if (bFreeToAnimate)
	{
		if (HasSkillAbility(InSkillType, InAbilityIndex))
		{
			auto AbilityData = GetSkillAbility(InSkillType, InAbilityIndex);
			if(HasWeapon(AbilityData.WeaponType))
			{
				if (ActiveAbility(AbilityData.AbilityHandle))
				{
					bAttacking = true;
					LimitToAnim(true, true);
					SetMotionRate(0, 0);
					SkillAbilityIndex = AbilityData.AbilityName;
					AttackType = EAttackType::SkillAttack;
					return true;
				}
			}
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
	switch (AttackType)
	{
		case EAttackType::NormalAttack:
			SetDamaging(true);
			break;
		case EAttackType::SkillAttack:
			if (GetSkillAbility(SkillAbilityIndex).GetItemData().SkillClass != nullptr)
			{
				FActorSpawnParameters spawnParams = FActorSpawnParameters();
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ASkill* tmpSkill = GetWorld()->SpawnActor<ASkill>(GetSkillAbility(SkillAbilityIndex).GetItemData().SkillClass, spawnParams);
				if(tmpSkill) tmpSkill->Initialize(this, SkillAbilityIndex);
			}
			break;
		case EAttackType::FallingAttack:
			SetDamaging(true);
			break;
	}
}

void ADWCharacter::AttackEnd()
{
	switch (AttackType)
	{
		case EAttackType::NormalAttack:
			SetDamaging(false);
			if (++AttackAbilityIndex >= AttackAbilitys.Num())
			{
				AttackAbilityIndex = 0;
			}
			break;
		case EAttackType::SkillAttack:
			UnAttack();
			break;
		case EAttackType::FallingAttack:
			UnAttack();
			StopAnimMontage();
			break;
	}
}

void ADWCharacter::UnAttack()
{
	if (bAttacking)
	{
		bAttacking = false;
		FreeToAnim(true);
		SetDamaging(false);
		AttackAbilityIndex = 0;
		SkillAbilityIndex = NAME_None;
		AttackType = EAttackType::None;
	}
}

void ADWCharacter::Defend()
{
	if (bFreeToAnimate)
	{
		bDefending = true;
		SetMotionRate(0.5f, 0.1f);
		LimitToAnim(true, true);
	}
}

void ADWCharacter::DefendStart()
{
	if (bDefending)
	{
		bBlocking = true;
	}
}

void ADWCharacter::DefendEnd()
{
	if (bDefending)
	{
		bBlocking = false;
	}
}

void ADWCharacter::UnDefend()
{
	if (bDefending)
	{
		bDefending = false;
		bBlocking = false;
		FreeToAnim(true);
	}
}

void ADWCharacter::OnDeathStart()
{
	DoAction(ECharacterActionType::Death);
}

void ADWCharacter::OnDeathEnd()
{
	bDead = true;
	bDying = false;
	SetVisible(false);
	GetCharacterMovement()->SetActive(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(Inventory) Inventory->DiscardAll();
	if (Nature != ECharacterNature::Player)
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
	}
}

void ADWCharacter::OnFallStart()
{
	bFalling = true;
	LimitToAnim();
	if(bDefending)
	{
		UnDefend();
	}
}

void ADWCharacter::OnFallEnd()
{
	bFalling = false;
	FreeToAnim();
	UnJump();
	if(bAttacking && AttackType == EAttackType::FallingAttack)
	{
		AttackEnd();
	}
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
			FGameplayModifierInfo ModifierInfo = Ability->GetCostGameplayEffect()->Modifiers[0];
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

	if(IsActive() || IsDead() && InActionType == ECharacterActionType::Death)
	{
		if (StopAction() && ActiveAbility(GetActionAbility(InActionType).AbilityHandle))
		{
			ActionType = InActionType;
			return true;
		}
	}
	return false;
}

bool ADWCharacter::StopAction(ECharacterActionType InActionType /*= ECharacterActionType::None*/, bool bCancelAbility /*= true*/)
{
	if (ActionType == ECharacterActionType::None) return true;
	
	if (InActionType == ECharacterActionType::None)
	{
		InActionType = ActionType;
	}
	else if(ActionType != InActionType || InActionType == ECharacterActionType::Death)
	{
		return false;
	}

	if (bCancelAbility)
	{
		CancelAbility(GetActionAbility(InActionType).AbilityHandle);
	}
	ActionType = ECharacterActionType::None;

	return true;
}

void ADWCharacter::ModifyEXP(float InDetlaValue)
{
	const int32 MaxEXP = GetMaxEXP();
	EXP += InDetlaValue;
	if (InDetlaValue > 0.f)
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

void ADWCharacter::ModifyHealth(float InDetlaValue)
{
	if(InDetlaValue < 0.f && GetHealth() > 0.f || InDetlaValue > 0.f && GetHealth() < GetMaxHealth())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetHealthAttribute(), EGameplayModOp::Additive, InDetlaValue);
	}
}

void ADWCharacter::ModifyMana(float InDetlaValue)
{
	if(InDetlaValue < 0.f && GetMana() > 0.f || InDetlaValue > 0.f && GetMana() < GetMaxMana())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetManaAttribute(), EGameplayModOp::Additive, InDetlaValue);
	}
}

void ADWCharacter::ModifyStamina(float InDetlaValue)
{
	if(InDetlaValue < 0.f && GetStamina() > 0.f || InDetlaValue > 0.f && GetStamina() < GetMaxStamina())
	{
		AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetStaminaAttribute(), EGameplayModOp::Additive, InDetlaValue);
	}
}

void ADWCharacter::LookAtTarget(ADWCharacter* InTargetCharacter)
{
	if(!InTargetCharacter || !InTargetCharacter->IsValidLowLevel()) return;
	FVector tmpDirection = InTargetCharacter->GetActorLocation() - GetActorLocation();
	SetActorRotation(FRotator(0, tmpDirection.ToOrientationRotator().Yaw, 0));
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

void ADWCharacter::AddMovementInput(FVector InWorldDirection, float InScaleValue, bool bForce)
{
	if (bBreakAllInput) return;

	Super::AddMovementInput(InWorldDirection, InScaleValue, bForce);

	if (bAutoJump)
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

bool ADWCharacter::HasBehaviorTree()
{
	return BehaviorTreeAsset != nullptr;
}

UBehaviorTree* ADWCharacter::GetBehaviorTree()
{
	if (!BehaviorTree)
	{
		BehaviorTree = DuplicateObject<UBehaviorTree>(BehaviorTreeAsset, this);
		if(BehaviorTree)
		{
			Blackboard = DuplicateObject<UDWAIBlackboard>(BlackboardAsset, this);
			BehaviorTree->BlackboardAsset = Blackboard;
		}
	}
	return BehaviorTree;
}

UWidgetCharacterHP* ADWCharacter::GetWidgetCharacterHPWidget()
{
	if (WidgetCharacterHP->GetUserWidgetObject())
	{
		return Cast<UWidgetCharacterHP>(WidgetCharacterHP->GetUserWidgetObject());
	}
	return nullptr;
}

FTeamData* ADWCharacter::GetTeamData() const
{
	return AWorldManager::GetCurrent()->GetTeamData(*TeamID);
}

void ADWCharacter::SetName(const FString& InName)
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
		return AttackAbilitys[InAbilityIndex == -1 ? AttackAbilityIndex : InAbilityIndex];
	}
	return FDWCharacterAttackAbilityData();
}

FDWCharacterSkillAbilityData ADWCharacter::GetSkillAbility(const FName& InSkillID)
{
	if(HasSkillAbility(InSkillID))
	{
		return SkillAbilitys[InSkillID];
	}
	return FDWCharacterSkillAbilityData();
}

FDWCharacterSkillAbilityData ADWCharacter::GetSkillAbility(ESkillType InSkillType, int32 InAbilityIndex)
{
	if(HasSkillAbility(InSkillType, InAbilityIndex))
	{
		TArray<FDWCharacterSkillAbilityData> Abilitys = TArray<FDWCharacterSkillAbilityData>();
		for (auto Iter : SkillAbilitys)
		{
			if(Iter.Value.GetItemData().SkillType == InSkillType)
			{
				Abilitys.Add(Iter.Value);
			}
		}
		return InAbilityIndex != -1 ? Abilitys[InAbilityIndex] : Abilitys[0];
	}
	return FDWCharacterSkillAbilityData();
}

FDWCharacterActionAbilityData ADWCharacter::GetActionAbility(ECharacterActionType InActionType /*= ECharacterActionType::None*/)
{
	if(InActionType == ECharacterActionType::None) InActionType = ActionType;
	if (HasActionAbility(InActionType))
	{
		return ActionAbilitys[InActionType];
	}
	return FDWCharacterActionAbilityData();
}

bool ADWCharacter::RaycastStep(FHitResult& OutHitResult)
{
	FVector rayStart = GetActorLocation() + FVector::DownVector * (GetHalfHeight() - GetCharacterMovement()->MaxStepHeight);
	FVector rayEnd = rayStart + MoveDirection * (GetRadius() + AWorldManager::GetWorldInfo().BlockSize * FMath::Clamp(MoveVelocity.Size() * 0.005f, 0.5f, 1.3f));
	return UKismetSystemLibrary::LineTraceSingle(this, rayStart, rayEnd, AWorldManager::GetCurrent()->GetGameTrace(EGameTraceType::Step), false, TArray<AActor*>(), EDrawDebugTrace::None, OutHitResult, true);
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
		if(!IsDead() && Nature != ECharacterNature::Player)
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
		SpawnWidgetWorldText(DeltaValue > 0.f ? EWorldTextType::Recover : (Nature == ECharacterNature::Player ? EWorldTextType::DamagePlayer : EWorldTextType::DamageOther), FString::FromInt(FMath::Abs(DeltaValue)));
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
