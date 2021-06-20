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
class ADWPlayerCharacterController;
class UPawnSensingComponent;
class ADWPlayerCharacterCameraManager;

/**
 * Íæ¼Ò½ÇÉ«
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float InteractDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStats")
	UVoxel* PreviewVoxel;

	// inputs
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInputs")
	bool bPressedAttack;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInputs")
	bool bPressedDefend;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterInputs")
	bool bPressedSprint;

	// camera

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraLookUpRate;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	FVector2D CameraDistanceRange;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UVoxelMeshComponent* VoxelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USkeletalMeshComponent* HammerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USceneCaptureComponent2D* PreviewCapture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USceneCaptureComponent2D* MiniMapCapture;

private:
	int32 AttackAbilityQueue;
	
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	void Tick(float DeltaTime) override;

	void Active(bool bResetStats = true) override;

	void Disable(bool bDisableMovement = false, bool bDisableCollision = false) override;

	void Revive() override;

	void Interrupt(float InDuration  = -1, bool bInPlayAnim  = false) override;

	void LookAtTarget(ADWCharacter* InTargetCharacter) override;

	FString GetHeadInfo() const override;

	void LoadData(FCharacterData InSaveData) override;

	FCharacterData ToData() override;

	void UnAttack() override;

	void AttackStart() override;

	void RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot) override;
	
	UFUNCTION(BlueprintCallable)
	void SetControlMode(EControlMode InControlMode);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EControlMode GetControlMode() const { return ControlMode; }

	UFUNCTION(BlueprintCallable)
	void UpdateVoxelMesh();
	
	void HandleNameChanged(const FString& NewValue) override;

	void HandleTeamIDChanged(const FString& NewValue) override;

	void HandleRaceIDChanged(const FString& NewValue) override;

	void HandleLevelChanged(int32 NewValue, int32 DeltaValue = 0) override;

	void HandleEXPChanged(int32 NewValue, int32 DeltaValue = 0) override;

	void HandleHealthChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleMaxHealthChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleManaChanged(float NewValue, float DeltaValue = 0.f) override;
	
	void HandleMaxManaChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleStaminaChanged(float NewValue, float DeltaValue = 0.f) override;
	
	void HandleMaxStaminaChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleMoveSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleSwimSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleRideSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleFlySpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleRotationSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleJumpForceChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleDodgeForceChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleAttackForceChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleRepulseForceChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleAttackSpeedChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleAttackCritRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleAttackStealRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleDefendRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleDefendScopeChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandlePhysicsDefRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleMagicDefRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleToughnessRateChanged(float NewValue, float DeltaValue = 0.f) override;

	void HandleRegenSpeedAttribute(float NewValue, float DeltaValue = 0.f) override;

	void HandleExpendSpeedAttribute(float NewValue, float DeltaValue = 0.f) override;

protected:
	void TurnCam(float InRate);

	void LookUpCam(float InRate);

	void MoveForward(float InValue);

	void MoveRight(float InValue);
	
	void ZoomNear();
	
	void ZoomFar();

	void ToggleControlMode();
	
	void ToggleFly();

	void ToggleCrouch();
		
	void ToggleAutoJump();

	void ToggleLockSightTarget();

	void OnJumpPressed();

	void OnJumpReleased();

	void OnDodgePressed();

	void OnDodgeReleased();

	void OnSprintPressed();

	void OnSprintReleased();

	void OnAttackDestroyPressed();

	void OnAttackDestroyRepeat();

	void OnAttackDestroyReleased();

	void OnDefendGeneratePressed();

	void OnDefendGenerateRepeat();

	void OnDefendGenerateReleased();

	void ReleaseSkillAbility1();

	void ReleaseSkillAbility2();

	void ReleaseSkillAbility3();

	void ReleaseSkillAbility4();

	void ToggleInventoryPanel();

	void UseInventoryItem();

	void DiscardInventoryItem();

	void PrevInventorySlot();

	void NextInventorySlot();

	void SelectInventorySlot1();
		
	void SelectInventorySlot2();
		
	void SelectInventorySlot3();
		
	void SelectInventorySlot4();
		
	void SelectInventorySlot5();
		
	void SelectInventorySlot6();
		
	void SelectInventorySlot7();
		
	void SelectInventorySlot8();
		
	void SelectInventorySlot9();
		
	void SelectInventorySlot10();

	void PauseOrContinueGame();

	bool RaycastVoxel(FVoxelHitResult& OutHitResult);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInteractDistance() const { return InteractDistance; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPressedAttack() const { return bPressedAttack; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPressedDefend() const { return bPressedDefend; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPressedSprint() const { return bPressedSprint; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCameraTurnRate() const { return CameraTurnRate; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCameraLookUpRate() const { return CameraLookUpRate; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector2D GetCameraDistanceRange() const { return CameraDistanceRange; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWPlayerCharacterController* GetPlayerController();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWPlayerCharacterCameraManager* GetCameraManager();
};
