// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.ADWCharacter

#pragma once

#include "DreamWorld.h"
#include "Character/Human/DWHumanCharacter.h"
#include "DWPlayerCharacter.generated.h"

class UVoxel;
class UInputComponent;
class USpringArmComponent;
class UCameraComponent;
class UVoxelMeshComponent;
class USkeletalMeshComponent;
class USceneCaptureComponent2D;
class ADWPlayerController;
class UPawnSensingComponent;
class ADWPlayerCharacterCameraManager;
class UTargetSystemComponent;

/**
 * ��ҽ�ɫ
 */
UCLASS(config=Game)
class ADWPlayerCharacter : public ADWHumanCharacter
{
	GENERATED_BODY()

public:
	ADWPlayerCharacter();

protected:
	// states

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStates")
	EControlMode ControlMode;
	
	// stats
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStats")
	FVoxelItem VoxelItem;

	// inputs
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInputs")
	bool bPressedAttack;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInputs")
	bool bPressedDefend;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UTargetSystemComponent* TargetSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FollowCamera;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UVoxelMeshComponent* VoxelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* HammerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneCaptureComponent2D* PreviewCapture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneCaptureComponent2D* MiniMapCapture;

private:
	int32 AttackAbilityQueue;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void LoadData(FCharacterSaveData InSaveData) override;

	virtual FCharacterSaveData ToData(bool bSaved = true) override;

	virtual void LoadRecordData(FPlayerRecordSaveData InRecordData);

	virtual FPlayerRecordSaveData ToRecordData(bool bSaved = true);

public:
	virtual void Active(bool bResetData = false) override;

	virtual void Disable(bool bDisableMovement = false, bool bDisableCollision = false) override;

	virtual void Spawn() override;

	virtual void Revive() override;

	virtual void Death(ADWCharacter* InKiller) override;

	virtual void DeathEnd() override;

	virtual void ResetData(bool bRefresh) override;

	virtual void Interrupt(float InDuration  = -1, bool bInPlayAnim  = false) override;

	virtual void Dodge() override;

	virtual void UnDodge() override;

	virtual void LookAtTarget(ADWCharacter* InTargetCharacter) override;

	virtual void UnAttack() override;

	virtual void AttackStart() override;

	virtual bool UseItem(FItem& InItem) override;

	virtual void RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot) override;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateVoxelMesh();
	
	virtual bool RaycastVoxel(FVoxelHitResult& OutHitResult) override;

	virtual bool DoInteract(IInteraction* InTarget, EInteractOption InInteractOption) override;

protected:
	virtual void ToggleControlMode();

	virtual void ToggleCrouch();

	virtual void ToggleLockTarget();

	virtual void OnDodgePressed();

	virtual void OnDodgeReleased();

	virtual void OnAttackDestroyPressed();

	virtual void OnAttackDestroyRepeat();

	virtual void OnAttackDestroyReleased();

	virtual void OnDefendGeneratePressed();

	virtual void OnDefendGenerateRepeat();

	virtual void OnDefendGenerateReleased();

	virtual void ReleaseSkillAbility1();

	virtual void ReleaseSkillAbility2();

	virtual void ReleaseSkillAbility3();

	virtual void ReleaseSkillAbility4();

	virtual void DoInteractOption1();

	virtual void DoInteractOption2();

	virtual void DoInteractOption3();

	virtual void DoInteractOption4();

	virtual void DoInteractOption5();

public:
	virtual void HandleNameChanged(const FString& NewValue) override;

	virtual void HandleTeamIDChanged(const FString& NewValue) override;

	virtual void HandleRaceIDChanged(const FString& NewValue) override;

	virtual void HandleLevelChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleEXPChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleHealthChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleMaxHealthChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleManaChanged(float NewValue, float DeltaValue = 0.f) override;
	
	virtual void HandleMaxManaChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleStaminaChanged(float NewValue, float DeltaValue = 0.f) override;
	
	virtual void HandleMaxStaminaChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleMoveSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleSwimSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleRideSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleFlySpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleRotationSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleJumpForceChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleDodgeForceChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleAttackForceChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleRepulseForceChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleAttackSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleAttackCritRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleAttackStealRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleDefendRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleDefendScopeChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandlePhysicsDefRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleMagicDefRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleToughnessRateChanged(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleRegenSpeedAttribute(float NewValue, float DeltaValue = 0.f) override;

	virtual void HandleExpendSpeedAttribute(float NewValue, float DeltaValue = 0.f) override;

public:
	virtual FString GetHeadInfo() const override;

	UFUNCTION(BlueprintPure)
	EControlMode GetControlMode() const { return ControlMode; }
		
	UFUNCTION(BlueprintCallable)
	virtual void SetControlMode(EControlMode InControlMode);
	
	UFUNCTION(BlueprintPure)
	float GetInteractDistance() const { return InteractDistance; }
	
	UFUNCTION(BlueprintPure)
	UTargetSystemComponent* GetTargetSystem() const { return TargetSystem; }

	UFUNCTION(BlueprintPure)
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintPure)
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }
		
	UFUNCTION(BlueprintPure)
	ADWPlayerController* GetPlayerController() const;

	virtual void SetInteractingTarget(IInteraction* InTarget) override;
};
