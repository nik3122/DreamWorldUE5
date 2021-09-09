// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Character.h"
#include "Vitality/Vitality.h"
#include "AbilitySystemInterface.h"
#include "DWCharacter.generated.h"

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

/**
 * 角色
 */
UCLASS()
class DREAMWORLD_API ADWCharacter : public ACharacter, public IVitality, public IAbilitySystemInterface
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
	bool bBlocking;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	ECharacterNature Nature;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	FString RaceID;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	FString TeamID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	int32 EXP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	int32 BaseEXP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	int32 EXPFactor;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float AttackDistance;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float FollowDistance;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float PatrolDistance;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float PatrolDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	bool bAutoJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	FInventoryData InventoryData;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float MovementRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float RotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FVector MoveVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FVector MoveDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	FVector BirthLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	AChunk* OwnerChunk;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	UVoxel* OverlapVoxel;
	
protected:
	// setup
				
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UDataTable* PassiveEffectTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UDataTable* AttackAbilityTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UDataTable* SkillAbilityTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UDataTable* ActionAbilityTable;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	UBehaviorTree* BehaviorTreeAsset;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	UDWAIBlackboard* BlackboardAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FDWCharacterAttackAbilityData FallingAttackAbility;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionStimuliSourceComponent* StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetCharacterHPComponent* WidgetCharacterHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDWCharacterAnim* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDWAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDWCharacterAttributeSet* AttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBehaviorTree* BehaviorTree;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDWAIBlackboard* Blackboard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacterInventory* Inventory;

protected:
	float AIMoveStopDistance;

	float DodgeRemainTime;

	float InterruptRemainTime;

	FVector AIMoveLocation;

	int32 AttackAbilityIndex;

	FName SkillAbilityIndex;

	EAttackType AttackType;

	ECharacterActionType ActionType;

	UPROPERTY()
	ADWCharacter* LockedTarget;
	
	UPROPERTY()
	TMap<EEquipPartType, AEquip*> Equips;

	TArray<FDWCharacterPassiveEffectData> PassiveEffects;

	TArray<FDWCharacterAttackAbilityData> AttackAbilitys;

	TMap<FName, FDWCharacterSkillAbilityData> SkillAbilitys;

	TMap<ECharacterActionType, FDWCharacterActionAbilityData> ActionAbilitys;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadData(FCharacterData InSaveData);

	UFUNCTION(BlueprintPure)
	virtual FCharacterData ToData(bool bSaved = true);

	UFUNCTION(BlueprintPure)
	bool HasTeam() const;

	UFUNCTION(BlueprintPure)
	bool IsTeamMate(ADWCharacter* InTargetCharacter) const;

	UFUNCTION(BlueprintPure)
	bool HasAttackAbility(int32 InAbilityIndex = -1);

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
	
	UFUNCTION(BlueprintCallable)
	bool ValidInteract(ADWCharacter* InTargetCharacter, ECharacterInteract InCharacterInteract);

	UFUNCTION(BlueprintCallable)
	float Distance(ADWCharacter* InTargetCharacter, bool bIgnoreRadius = true, bool bIgnoreZAxis = true);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetVisible(bool bVisible);
									
	UFUNCTION(BlueprintCallable)
	void SetMotionRate(float InMovementRate, float InRotationRate);

	UFUNCTION(BlueprintCallable)
	virtual void Active(bool bResetStats = false);
	
	UFUNCTION(BlueprintCallable)
	virtual void Disable(bool bDisableMovement = false, bool bDisableCollision = false);
			
	UFUNCTION(BlueprintCallable)
	virtual void Death(ADWCharacter* InKiller = nullptr) override;
				
	UFUNCTION(BlueprintCallable)
	virtual void Revive() override;

	UFUNCTION(BlueprintCallable)
	virtual void Refresh() override;

	UFUNCTION(BlueprintCallable)
	virtual void FreeToAnim(bool bResetStats = false);

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
	virtual void Climb();

	UFUNCTION(BlueprintCallable)
	virtual void UnClimb();
			
	UFUNCTION(BlueprintCallable)
	virtual void Ride();

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
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void UnAttack();

	UFUNCTION(BlueprintCallable)
	virtual void Defend();

	UFUNCTION(BlueprintCallable)
	virtual void DefendStart();

	UFUNCTION(BlueprintCallable)
	virtual void DefendEnd();

	UFUNCTION(BlueprintCallable)
	virtual void UnDefend();
			
	UFUNCTION(BlueprintCallable)
	virtual void OnDeathStart();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnDeathEnd();

	UFUNCTION(BlueprintCallable)
	virtual void OnFallStart();

	UFUNCTION(BlueprintCallable)
	virtual void OnFallEnd();
	
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
	
	UFUNCTION(BlueprintCallable)
	bool RemoveEffect(FActiveGameplayEffectHandle Handle, int32 StacksToRemove=-1);

	UFUNCTION(BlueprintPure)
	virtual void GetActiveAbilities(FGameplayTagContainer AbilityTags, TArray<UDWGameplayAbility*>& ActiveAbilities);

	UFUNCTION(BlueprintPure)
	virtual bool GetAbilityInfo(TSubclassOf<UDWGameplayAbility> AbilityClass, FDWAbilityInfo& OutAbilityInfo);

	UFUNCTION(BlueprintCallable)
	virtual bool DoAction(ECharacterActionType InActionType);
	
	UFUNCTION(BlueprintCallable)
	virtual bool StopAction(ECharacterActionType InActionType = ECharacterActionType::None, bool bCancelAbility = true);
				
	UFUNCTION(BlueprintCallable)
	virtual void ModifyEXP(float InDetlaValue);
				
	UFUNCTION(BlueprintCallable)
	virtual void ModifyHealth(float InDetlaValue) override;
					
	UFUNCTION(BlueprintCallable)
	virtual void ModifyMana(float InDetlaValue);
									
	UFUNCTION(BlueprintCallable)
	virtual void ModifyStamina(float InDetlaValue);

	UFUNCTION(BlueprintCallable)
	virtual void LookAtTarget(ADWCharacter* InTargetCharacter);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetLockedTarget(ADWCharacter* InTargetCharacter);
		
	UFUNCTION(BlueprintCallable)
	virtual void AIMoveTo(FVector InTargetLocation, float InMoveStopDistance = 10);

	UFUNCTION(BlueprintCallable)
	virtual bool DoAIMove(FVector InTargetLocation, float InMoveStopDistance = 10);
	
	virtual bool DoAIMove(ADWCharacter* InTargetCharacter, float InMoveStopDistance = 10, bool bLookAtTarget = false);

	UFUNCTION(BlueprintCallable)
	virtual void StopAIMove();

protected:
	virtual void AddMovementInput(FVector InWorldDirection, float InScaleValue = 1, bool bForce = false) override;

	virtual void SetDamaging(bool bInDamaging);

	virtual bool RaycastStep(FHitResult& OutHitResult);

public:
	UFUNCTION(BlueprintPure)
	UAIPerceptionStimuliSourceComponent* GetStimuliSource() const { return StimuliSource; }
	
	UFUNCTION(BlueprintPure)
	UWidgetCharacterHPComponent* GetWidgetCharacterHP() const { return WidgetCharacterHP; }
	
	UFUNCTION(BlueprintPure)
	UDWCharacterAnim* GetAnimInstance() const { return AnimInstance; }
	
	UFUNCTION(BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UFUNCTION(BlueprintPure)
	UDWCharacterAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure)
	bool HasBehaviorTree();

	UFUNCTION(BlueprintPure)
	UBehaviorTree* GetBehaviorTree();
	
	UFUNCTION(BlueprintPure)
	UWidgetCharacterHP* GetWidgetCharacterHPWidget();

public:
	UFUNCTION(BlueprintPure)
	bool IsActive(bool bNeedFreeToAnim = false) const;
														
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
	bool IsAttacking() const { return bAttacking; }

	UFUNCTION(BlueprintPure)
	bool IsDefending() const { return bDefending; }

	UFUNCTION(BlueprintPure)
	bool IsDamaging() const { return bDamaging; }

	UFUNCTION(BlueprintPure)
	bool IsBlocking() const { return bBlocking; }

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
	bool IsFreeToAnimate() const { return bFreeToAnimate; }

	UFUNCTION(BlueprintPure)
	bool IsBreakAllInput() const { return bBreakAllInput; }

public:
	UFUNCTION(BlueprintPure)
	FString GetName() const override { return Name; }

	UFUNCTION(BlueprintCallable)
	virtual void SetName(const FString& InName) override;
	
	UFUNCTION(BlueprintPure)
	ECharacterNature GetNature() const { return Nature; }
			
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
	UCharacterInventory* GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintPure)
	FVector GetMoveVelocity() const { return MoveVelocity; }

	UFUNCTION(BlueprintPure)
	FVector GetMoveDirection() const { return MoveDirection; }

	UFUNCTION(BlueprintPure)
	AChunk* GetOwnerChunk() const { return OwnerChunk; }

	UFUNCTION(BlueprintCallable)
	void SetOwnerChunk(AChunk* val) { OwnerChunk = val; }

	UFUNCTION(BlueprintPure)
	UVoxel* GetOverlapVoxel() const { return OverlapVoxel; }

	UFUNCTION(BlueprintCallable)
	void SetOverlapVoxel(UVoxel* val) { OverlapVoxel = val; }

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
	bool IsAutoJump() const { return bAutoJump; }

	UFUNCTION(BlueprintCallable)
	void SetAutoJump(bool val) { bAutoJump = val; }
		
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
	TArray<FDWCharacterAttackAbilityData> GetAttackAbilitys() const { return AttackAbilitys; }

	UFUNCTION(BlueprintPure)
	TMap<FName, FDWCharacterSkillAbilityData> GetSkillAbilitys() const { return SkillAbilitys; }

	UFUNCTION(BlueprintPure)
	TMap<ECharacterActionType, FDWCharacterActionAbilityData> GetActionAbilitys() const { return ActionAbilitys; }

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
