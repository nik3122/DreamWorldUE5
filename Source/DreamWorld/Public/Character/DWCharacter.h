// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Character.h"
#include "Vitality/Vitality.h"
#include "AbilitySystemInterface.h"
#include "Interaction.h"
#include "TargetSystemComponent.h"
#include "TargetSystemTargetableInterface.h"

#include "DWCharacter.generated.h"

class UInteractionComponent;
class UBoxComponent;
class UDWCharacterPart;
class AEquipArmor;
class AEquipShield;
class AEquipWeapon;
class UInventoryEquipSlot;
class AChunk;
class UVoxel;
class AController;
class UDWCharacterAnim;
class UWidgetCharacterHPComponent;
class UDWGameplayAbility;
class UDWAbilitySystemComponent;
class UAIPerceptionStimuliSourceComponent;
class UCharacterInventory;
class UBehaviorTree;
class UDWAIBlackboard;
class UInventorySlot;
class ASkill;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

/**
 * 角色
 */
UCLASS()
class DREAMWORLD_API ADWCharacter : public ACharacter, public IVitality, public IInteraction, public IAbilitySystemInterface, public ITargetSystemTargetableInterface
{
	GENERATED_BODY()

public:
	ADWCharacter();
	
protected:
	// states

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDying;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bActive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDodging;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bSprinting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bCrouching;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bSwimming;
				
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFloating;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bClimbing;
					
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bRiding;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFlying;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bAttacking;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDefending;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDamaging;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bInterrupting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bLockRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFreeToAnimate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bBreakAllInput;

protected:
	// stats
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FName ID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	ECharacterNature Nature;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FString RaceID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FString TeamID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	int32 EXP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	int32 BaseEXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	int32 EXPFactor;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float AttackDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float InteractDistance;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float FollowDistance;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float PatrolDistance;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float PatrolDuration;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float MovementRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float RotationRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FVector BirthLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	AChunk* OwnerChunk;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	AController* OwnerController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	ADWCharacter* OwnerRider;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	ADWCharacter* RidingTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	ADWCharacter* LockedTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	TArray<EInteractOption> InteractOptions;

	IInteraction* InteractingTarget;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionStimuliSourceComponent* StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetCharacterHPComponent* WidgetCharacterHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInteractionComponent* Interaction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDWCharacterAnim* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDWAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDWCharacterAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacterInventory* Inventory;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDead OnCharacterDead;

protected:
	float DefaultGravityScale;

	float DefaultAirControl;
	
	float AIMoveStopDistance;

	float InterruptRemainTime;

	float NormalAttackRemainTime;

	FVector MoveDirection;

	FVector AIMoveLocation;

	int32 AttackAbilityIndex;

	FName SkillAbilityIndex;

	EAttackType AttackType;

	ECharacterActionType ActionType;

	FTimerHandle AttackHurtTimer;

	UPROPERTY()
	TMap<EEquipPartType, AEquip*> Equips;

	FDWAbilityData DefaultAbility;
	
	FDWCharacterAttackAbilityData FallingAttackAbility;

	TArray<FDWCharacterAttackAbilityData> AttackAbilities;

	TMap<FName, FDWCharacterSkillAbilityData> SkillAbilities;

	TMap<ECharacterActionType, FDWCharacterActionAbilityData> ActionAbilities;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Serialize(FArchive& Ar) override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadData(FCharacterSaveData InSaveData);

	UFUNCTION(BlueprintPure)
	virtual FCharacterSaveData ToData(bool bSaved = true);

public:
	UFUNCTION(BlueprintCallable)
	virtual void Active(bool bResetData = false);
	
	UFUNCTION(BlueprintCallable)
	virtual void Disable(bool bDisableMovement = false, bool bDisableCollision = false);
						
	UFUNCTION(BlueprintCallable)
	virtual void Spawn() override;
		
	UFUNCTION(BlueprintCallable)
	virtual void Revive() override;
			
	UFUNCTION(BlueprintCallable)
	virtual void Death(ADWCharacter* InKiller = nullptr) override;
			
	virtual void DeathStart();

	UFUNCTION(BlueprintCallable)
	virtual void DeathEnd();

	virtual bool DoInteract(IInteraction* InTarget, EInteractOption InInteractOption);

	virtual bool OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption) override;

	UFUNCTION(BlueprintCallable)
	virtual void ResetData(bool bRefresh = false) override;

	UFUNCTION(BlueprintCallable)
	virtual void RefreshData() override;

	UFUNCTION(BlueprintCallable)
	virtual void FreeToAnim(bool bUnLockRotation = true);

	UFUNCTION(BlueprintCallable)
	virtual void LimitToAnim(bool bInLockRotation = false, bool bUnSprint = false);

	UFUNCTION(BlueprintCallable)
	virtual void Interrupt(float InDuration = -1, bool bDoAction = false);
			
	UFUNCTION(BlueprintCallable)
	virtual void UnInterrupt();
	
	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	virtual void UnJump();

	UFUNCTION(BlueprintCallable)
	virtual void Dodge();

	UFUNCTION(BlueprintCallable)
	virtual void UnDodge();

	UFUNCTION(BlueprintCallable)
	virtual void Sprint();

	UFUNCTION(BlueprintCallable)
	virtual void UnSprint();

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false) override;
		
	UFUNCTION(BlueprintCallable)
	virtual void Swim();

	UFUNCTION(BlueprintCallable)
	virtual void UnSwim();
								
	UFUNCTION(BlueprintCallable)
	virtual void Float(float InWaterPosZ);
	
	UFUNCTION(BlueprintCallable)
	virtual void UnFloat();

	UFUNCTION(BlueprintCallable)
	virtual void Climb();

	UFUNCTION(BlueprintCallable)
	virtual void UnClimb();
			
	UFUNCTION(BlueprintCallable)
	virtual void Ride(ADWCharacter* InTarget);

	UFUNCTION(BlueprintCallable)
	virtual void UnRide();

	UFUNCTION(BlueprintCallable)
	virtual void Fly();

	UFUNCTION(BlueprintCallable)
	virtual void UnFly();

	UFUNCTION(BlueprintCallable)
	virtual bool Attack(int32 InAbilityIndex = -1);

	UFUNCTION(BlueprintCallable)
	virtual bool SkillAttack(const FName& InSkillID);

	virtual bool SkillAttack(ESkillType InSkillType, int32 InAbilityIndex = -1);
		
	UFUNCTION(BlueprintCallable)
	virtual bool FallingAttack();

	UFUNCTION(BlueprintCallable)
	virtual void AttackStart();

	UFUNCTION(BlueprintCallable)
	virtual void AttackHurt();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void UnAttack();

	UFUNCTION(BlueprintCallable)
	virtual void Defend();

	virtual void UnDefend();

	virtual void FallStart();

	virtual void FallEnd();
	
	virtual bool UseItem(FItem& InItem);

	UFUNCTION(BlueprintCallable)
	virtual bool GenerateVoxel(const FVoxelHitResult& InVoxelHitResult, FItem& InItem);

	UFUNCTION(BlueprintCallable)
	virtual bool DestroyVoxel(const FVoxelHitResult& InVoxelHitResult);

	UFUNCTION(BlueprintCallable)
	virtual void RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot);

	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpecHandle AcquireAbility(TSubclassOf<UDWGameplayAbility> InAbility, int32 InLevel = 1) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ActiveAbility(FGameplayAbilitySpecHandle AbilityHandle, bool bAllowRemoteActivation = false) override;

	virtual bool ActiveAbility(TSubclassOf<UDWGameplayAbility> AbilityClass, bool bAllowRemoteActivation = false) override;

	virtual bool ActiveAbility(const FGameplayTagContainer& AbilityTags, bool bAllowRemoteActivation = false) override;

	UFUNCTION(BlueprintCallable)
	virtual void CancelAbility(UDWGameplayAbility* Ability) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle& AbilityHandle) override;

	//UFUNCTION(BlueprintCallable)
	virtual void CancelAbilities(const FGameplayTagContainer* WithTags=nullptr, const FGameplayTagContainer* WithoutTags=nullptr, UDWGameplayAbility* Ignore=nullptr) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void CancelAllAbilities(UDWGameplayAbility* Ignore=nullptr) override;
	
	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle ApplyEffect(TSubclassOf<UGameplayEffect> EffectClass);
	
	FActiveGameplayEffectHandle ApplyEffect(const FGameplayEffectSpecHandle& SpecHandle);
		
	FActiveGameplayEffectHandle ApplyEffect(const FGameplayEffectSpec& GameplayEffect);

	UFUNCTION(BlueprintCallable)
	bool RemoveEffect(FActiveGameplayEffectHandle Handle, int32 StacksToRemove=-1);

	UFUNCTION(BlueprintPure)
	virtual void GetActiveAbilities(FGameplayTagContainer AbilityTags, TArray<UDWGameplayAbility*>& ActiveAbilities);

	UFUNCTION(BlueprintPure)
	virtual bool GetAbilityInfo(TSubclassOf<UDWGameplayAbility> AbilityClass, FDWAbilityInfo& OutAbilityInfo);

	UFUNCTION(BlueprintCallable)
	virtual bool DoAction(ECharacterActionType InActionType);

	UFUNCTION(BlueprintCallable)
	virtual void EndAction(ECharacterActionType InActionType);

	UFUNCTION(BlueprintCallable)
	virtual bool StopAction(ECharacterActionType InActionType = ECharacterActionType::None, bool bCancelAbility = true, bool bEndAction = false);
				
	UFUNCTION(BlueprintCallable)
	virtual void ModifyEXP(float InDeltaValue) override;
				
	UFUNCTION(BlueprintCallable)
	virtual void ModifyHealth(float InDeltaValue) override;
					
	UFUNCTION(BlueprintCallable)
	virtual void ModifyMana(float InDeltaValue);
									
	UFUNCTION(BlueprintCallable)
	virtual void ModifyStamina(float InDeltaValue);

	UFUNCTION(BlueprintCallable)
	virtual void SetLockedTarget(ADWCharacter* InTargetCharacter);
		
	UFUNCTION(BlueprintCallable)
	virtual void LookAtTarget(ADWCharacter* InTargetCharacter);

	UFUNCTION(BlueprintCallable)
	virtual void AIMoveTo(FVector InTargetLocation, float InMoveStopDistance = 10);

	UFUNCTION(BlueprintCallable)
	virtual bool DoAIMove(FVector InTargetLocation, float InMoveStopDistance = 10);
	
	virtual bool DoAIMove(ADWCharacter* InTargetCharacter, float InMoveStopDistance = 10, bool bLookAtTarget = false);

	UFUNCTION(BlueprintCallable)
	virtual void StopAIMove();

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	virtual void SetDamaging(bool bInDamaging);

	virtual bool RaycastStep(FHitResult& OutHitResult);

	virtual bool RaycastVoxel(FVoxelHitResult& OutHitResult);

public:
	UFUNCTION(BlueprintPure)
	bool HasTeam() const;

	UFUNCTION(BlueprintPure)
	bool IsTeamMate(ADWCharacter* InTargetCharacter) const;

	UFUNCTION(BlueprintPure)
	bool HasAttackAbility(int32 InAbilityIndex = -1) const;

	UFUNCTION(BlueprintCallable)
	bool HasSkillAbility(const FName& InSkillID);

	bool HasSkillAbility(ESkillType InSkillType, int32 InAbilityIndex = -1);
	
	UFUNCTION(BlueprintCallable)
	bool HasActionAbility(ECharacterActionType InActionType);

	UFUNCTION(BlueprintCallable)
	bool CreateTeam(const FName& InTeamName = NAME_None, FString InTeamDetail = TEXT(""));
		
	UFUNCTION(BlueprintCallable)
	bool DissolveTeam();

	UFUNCTION(BlueprintCallable)
	bool JoinTeam(const FName& InTeamID);
		
	bool JoinTeam(ADWCharacter* InTargetCharacter);

	UFUNCTION(BlueprintCallable)
	bool LeaveTeam();
		
	UFUNCTION(BlueprintCallable)
	bool AddTeamMate(ADWCharacter* InTargetCharacter);
	
	UFUNCTION(BlueprintCallable)
	bool RemoveTeamMate(ADWCharacter* InTargetCharacter);

	UFUNCTION(BlueprintCallable)
	TArray<ADWCharacter*> GetTeamMates();

	UFUNCTION(BlueprintCallable)
	void SpawnWidgetWorldText(EWorldTextType InContextType, FString InContext) override;
	
	UFUNCTION(BlueprintPure)
	bool IsPlayer() const;
	
	UFUNCTION(BlueprintPure)
	bool IsEnemy(ADWCharacter* InTargetCharacter) const;

	UFUNCTION(BlueprintCallable)
	float Distance(ADWCharacter* InTargetCharacter, bool bIgnoreRadius = true, bool bIgnoreZAxis = true);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetVisible(bool bVisible);
									
	UFUNCTION(BlueprintCallable)
	void SetMotionRate(float InMovementRate, float InRotationRate);

public:
	UFUNCTION(BlueprintPure)
	UAIPerceptionStimuliSourceComponent* GetStimuliSource() const { return StimuliSource; }
	
	UFUNCTION(BlueprintPure)
	UWidgetCharacterHPComponent* GetWidgetCharacterHP() const { return WidgetCharacterHP; }

	UFUNCTION(BlueprintPure)
	UDWCharacterAnim* GetAnimInstance() const { return AnimInstance; }
	
	UFUNCTION(BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UDWCharacterAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure)
	bool HasBehaviorTree() const;

	UFUNCTION(BlueprintPure)
	UBehaviorTree* GetBehaviorTree();
	
	UFUNCTION(BlueprintPure)
	UWidgetCharacterHP* GetWidgetCharacterHPWidget() const;

public:
	UFUNCTION(BlueprintPure)
	bool IsActive(bool bNeedFreeToAnim = false) const;
																	
	UFUNCTION(BlueprintPure)
	bool IsFreeToAnim(bool bCheckStates = true) const;
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const override;

	UFUNCTION(BlueprintPure)
	bool IsDying() const { return bDying; }

	UFUNCTION(BlueprintPure)
	bool IsFalling() const { return bFalling; }
	
	UFUNCTION(BlueprintPure)
	bool IsDodging() const { return bDodging; }

	UFUNCTION(BlueprintPure)
	bool IsSprinting() const { return bSprinting; }

	UFUNCTION(BlueprintPure)
	bool IsCrouching() const { return bCrouching; }

	UFUNCTION(BlueprintPure)
	bool IsSwimming() const { return bSwimming; }

	UFUNCTION(BlueprintPure)
	bool IsFloating() const { return bFloating; }

	UFUNCTION(BlueprintPure)
	bool IsAttacking() const { return bAttacking; }

	UFUNCTION(BlueprintPure)
	bool IsDefending() const { return bDefending; }

	UFUNCTION(BlueprintPure)
	bool IsDamaging() const { return bDamaging; }

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const { return bClimbing; }

	UFUNCTION(BlueprintPure)
	bool IsRiding() const { return bRiding; }

	UFUNCTION(BlueprintPure)
	bool IsFlying() const { return bFlying; }

	UFUNCTION(BlueprintPure)
	bool IsInterrupting() const { return bInterrupting; }

	UFUNCTION(BlueprintPure)
	bool IsLockRotation() const { return bLockRotation; }

	UFUNCTION(BlueprintPure)
	bool IsBreakAllInput() const { return bBreakAllInput; }
	
	virtual bool IsTargetable_Implementation() const override;

	virtual TArray<EInteractOption> GetInteractOptions(IInteraction* InTrigger) const override;

	virtual IInteraction* GetInteractingTarget() const override { return InteractingTarget; }

	virtual void SetInteractingTarget(IInteraction* InTarget) override { InteractingTarget = InTarget; }

public:
	UFUNCTION(BlueprintPure)
	FString GetNameC() const override { return Name; }

	UFUNCTION(BlueprintCallable)
	virtual void SetNameC(const FString& InName) override;
	
	UFUNCTION(BlueprintPure)
	ECharacterNature GetNature() const { return Nature; }
						
	UFUNCTION(BlueprintPure)
	FCharacterData GetCharacterData() const;

	//UFUNCTION(BlueprintPure)
	FTeamData* GetTeamData() const;

	UFUNCTION(BlueprintPure)
	FString GetTeamID() const { return TeamID; }

	UFUNCTION(BlueprintCallable)
	void SetTeamID(const FString& InTeamID);

	UFUNCTION(BlueprintPure)
	FString GetRaceID() const override { return RaceID; }

	UFUNCTION(BlueprintCallable)
	void SetRaceID(const FString& InRaceID) override;

	UFUNCTION(BlueprintPure)
	int32 GetLevelC() const override { return Level; }
	
	UFUNCTION(BlueprintCallable)
	void SetLevelC(int32 InLevel) override;

	UFUNCTION(BlueprintPure)
	int32 GetEXP() const override { return EXP; }
		
	UFUNCTION(BlueprintCallable)
	void SetEXP(int32 InEXP) override;
	
	UFUNCTION(BlueprintPure)
	int32 GetBaseEXP() const override { return BaseEXP; }
	
	UFUNCTION(BlueprintPure)
	int32 GetEXPFactor() const override { return EXPFactor; }

	UFUNCTION(BlueprintPure)
	int32 GetMaxEXP() const override;

	UFUNCTION(BlueprintPure)
	int32 GetTotalEXP() const override;

	UFUNCTION(BlueprintPure)
	virtual FString GetHeadInfo() const override;
		
	UFUNCTION(BlueprintPure)
	float GetHealth() const override;
		
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InValue) override;

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const override;
		
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float InValue) override;
			
	UFUNCTION(BlueprintPure)
	float GetMana() const;
			
	UFUNCTION(BlueprintCallable)
	void SetMana(float InValue);

	UFUNCTION(BlueprintPure)
	float GetMaxMana() const;
			
	UFUNCTION(BlueprintCallable)
	void SetMaxMana(float InValue);
		
	UFUNCTION(BlueprintPure)
	float GetStamina() const;
		
	UFUNCTION(BlueprintCallable)
	void SetStamina(float InValue);

	UFUNCTION(BlueprintPure)
	float GetMaxStamina() const;
		
	UFUNCTION(BlueprintCallable)
	void SetMaxStamina(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetMoveSpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetMoveSpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetSwimSpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetSwimSpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetRideSpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetRideSpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetFlySpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetFlySpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetRotationSpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetRotationSpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetJumpForce() const;
		
	UFUNCTION(BlueprintCallable)
	void SetJumpForce(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetDodgeForce() const;
		
	UFUNCTION(BlueprintCallable)
	void SetDodgeForce(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetAttackForce() const;
		
	UFUNCTION(BlueprintCallable)
	void SetAttackForce(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetRepulseForce() const;
		
	UFUNCTION(BlueprintCallable)
	void SetRepulseForce(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetAttackSpeed() const;
		
	UFUNCTION(BlueprintCallable)
	void SetAttackSpeed(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetAttackCritRate() const;
		
	UFUNCTION(BlueprintCallable)
	void SetAttackCritRate(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetAttackStealRate() const;
		
	UFUNCTION(BlueprintCallable)
	void SetAttackStealRate(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetDefendRate() const;
		
	UFUNCTION(BlueprintCallable)
	void SetDefendRate(float InValue);

	UFUNCTION(BlueprintPure)
	float GetDefendScope() const;
		
	UFUNCTION(BlueprintCallable)
	void SetDefendScope(float InValue);

	UFUNCTION(BlueprintPure)
	float GetPhysicsDefRate() const;
		
	UFUNCTION(BlueprintCallable)
	void SetPhysicsDefRate(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetMagicDefRate() const;
		
	UFUNCTION(BlueprintCallable)
	void SetMagicDefRate(float InValue);
			
	UFUNCTION(BlueprintPure)
	float GetToughnessRate() const;
				
	UFUNCTION(BlueprintCallable)
	void SetToughnessRate(float InValue);
	
	UFUNCTION(BlueprintPure)
	float GetStaminaRegenSpeed() const;
			
	UFUNCTION(BlueprintCallable)
	void SetStaminaRegenSpeed(float InValue);
		
	UFUNCTION(BlueprintPure)
	float GetStaminaExpendSpeed() const;
	
	UFUNCTION(BlueprintCallable)
	void SetStaminaExpendSpeed(float InValue);

	UFUNCTION(BlueprintPure)
	float GetPhysicsDamage() const override;
	
	UFUNCTION(BlueprintPure)
	float GetMagicDamage() const override;

	UFUNCTION(BlueprintPure)
	class UInventory* GetInventory() const override;

	UFUNCTION(BlueprintPure)
	FVector GetMoveVelocity(bool bIgnoreZ = true) const;

	UFUNCTION(BlueprintPure)
	FVector GetMoveDirection(bool bIgnoreZ = true) const;

	UFUNCTION(BlueprintPure)
	AChunk* GetOwnerChunk() const { return OwnerChunk; }

	UFUNCTION(BlueprintCallable)
	void SetOwnerChunk(AChunk* val) { OwnerChunk = val; }
	
	UFUNCTION(BlueprintPure)
	AController* GetOwnerController() const { return OwnerController; }

	UFUNCTION(BlueprintCallable)
	void SetOwnerController(AController* InOwnerController) { this->OwnerController = InOwnerController; }

	UFUNCTION(BlueprintPure)
	ADWCharacter* GetOwnerRider() const { return OwnerRider; }

	UFUNCTION(BlueprintCallable)
	void SetOwnerRider(ADWCharacter* InOwnerRider) { this->OwnerRider = InOwnerRider; }

	UFUNCTION(BlueprintPure)
	ADWCharacter* GetRidingTarget() const { return RidingTarget; }

	UFUNCTION(BlueprintPure)
	ADWCharacter* GetLockedTarget() const { return LockedTarget; }

	UFUNCTION(BlueprintPure)
	FVector GetBirthLocation() const { return BirthLocation; }

	UFUNCTION(BlueprintPure)
	float GetAttackDistance() const { return AttackDistance; }

	UFUNCTION(BlueprintPure)
	float GetFollowDistance() const { return FollowDistance; }

	UFUNCTION(BlueprintPure)
	float GetPatrolDistance() const { return PatrolDistance; }

	UFUNCTION(BlueprintPure)
	float GetPatrolDuration() const { return PatrolDuration; }
		
	UFUNCTION(BlueprintPure)
	float GetRadius() const;

	UFUNCTION(BlueprintPure)
	float GetHalfHeight() const;
	
	UFUNCTION(BlueprintPure)
	bool HasWeapon(EWeaponType InWeaponType);
		
	UFUNCTION(BlueprintPure)
	bool HasShield(EShieldType InShieldType);
		
	UFUNCTION(BlueprintPure)
	AEquipWeapon* GetWeapon();
	
	UFUNCTION(BlueprintPure)
	AEquipShield* GetShield();
		
	UFUNCTION(BlueprintPure)
	bool HasArmor(EEquipPartType InPartType);
	
	UFUNCTION(BlueprintPure)
	AEquipArmor* GetArmor(EEquipPartType InPartType);

	UFUNCTION(BlueprintPure)
	bool HasEquip(EEquipPartType InPartType);
	
	UFUNCTION(BlueprintPure)
	AEquip* GetEquip(EEquipPartType InPartType);
	
	UFUNCTION(BlueprintPure)
	TMap<EEquipPartType, AEquip*> GetEquips() const { return Equips; }

	UFUNCTION(BlueprintPure)
	FDWCharacterAttackAbilityData GetAttackAbility(int32 InAbilityIndex = -1);
		
	UFUNCTION(BlueprintPure)
	FDWCharacterSkillAbilityData GetSkillAbility(const FName& InSkillID);
		
	FDWCharacterSkillAbilityData GetSkillAbility(ESkillType InSkillType, int32 InAbilityIndex = -1);
			
	UFUNCTION(BlueprintPure)
	FDWCharacterActionAbilityData GetActionAbility(ECharacterActionType InActionType = ECharacterActionType::None);

	UFUNCTION(BlueprintPure)
	TArray<FDWCharacterAttackAbilityData> GetAttackAbilities() const { return AttackAbilities; }

	UFUNCTION(BlueprintPure)
	TMap<FName, FDWCharacterSkillAbilityData> GetSkillAbilities() const { return SkillAbilities; }

	UFUNCTION(BlueprintPure)
	TMap<ECharacterActionType, FDWCharacterActionAbilityData> GetActionAbilities() const { return ActionAbilities; }

	UFUNCTION(BlueprintPure)
	UDWCharacterPart* GetCharacterPart(ECharacterPartType InCharacterPartType) const;

public:
	virtual void HandleDamage(const float LocalDamageDone, FHitResult HitResult, const struct FGameplayTagContainer& SourceTags, ADWCharacter* SourceCharacter, AActor* SourceActor) override;
	
	virtual void HandleInterrupt(float InterruptDuration);
	
	virtual void HandleNameChanged(const FString& NewValue) override;

	virtual void HandleTeamIDChanged(const FString& NewValue);

	virtual void HandleRaceIDChanged(const FString& NewValue) override;

	virtual void HandleLevelChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleEXPChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleHealthChanged(float NewValue, float DeltaValue = 0.f) override;
	
	virtual void HandleMaxHealthChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleManaChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleMaxManaChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleStaminaChanged(float NewValue, float DeltaValue = 0.f);
	
	virtual void HandleMaxStaminaChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleMoveSpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleSwimSpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleRideSpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleFlySpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleRotationSpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleJumpForceChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleDodgeForceChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleAttackForceChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleRepulseForceChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleAttackSpeedChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleAttackCritRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleAttackStealRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleDefendRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleDefendScopeChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandlePhysicsDefRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleMagicDefRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleToughnessRateChanged(float NewValue, float DeltaValue = 0.f);

	virtual void HandleRegenSpeedAttribute(float NewValue, float DeltaValue = 0.f);

	virtual void HandleExpendSpeedAttribute(float NewValue, float DeltaValue = 0.f);
};
