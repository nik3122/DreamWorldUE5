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

	virtual void Active(bool bResetStats = false) override;

	virtual void Disable(bool bDisableMovement = false, bool bDisableCollision = false) override;

	virtual void Revive() override;

	virtual void Interrupt(float InDuration  = -1, bool bInPlayAnim  = false) override;

	virtual void LookAtTarget(ADWCharacter* InTargetCharacter) override;

	virtual FString GetHeadInfo() const override;

	virtual void LoadData(FCharacterData InSaveData) override;

	virtual FCharacterData ToData(bool bSaved = true) override;

	virtual void UnAttack() override;

	virtual void AttackStart() override;

	virtual void RefreshEquip(EEquipPartType InPartType, UInventoryEquipSlot* EquipSlot) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetControlMode(EControlMode InControlMode);
	
	UFUNCTION(BlueprintPure)
	EControlMode GetControlMode() const { return ControlMode; }

	UFUNCTION(BlueprintCallable)
	virtual void UpdateVoxelMesh();
	
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

protected:
	virtual void TurnCam(float InRate);

	virtual void LookUpCam(float InRate);

	virtual void MoveForward(float InValue);

	virtual void MoveRight(float InValue);
	
	virtual void ZoomNear();
	
	virtual void ZoomFar();

	virtual void ToggleControlMode();
	
	virtual void ToggleFly();

	virtual void ToggleCrouch();
		
	virtual void ToggleAutoJump();

	virtual void ToggleLockSightTarget();

	virtual void OnJumpPressed();

	virtual void OnJumpReleased();

	virtual void OnDodgePressed();

	virtual void OnDodgeReleased();

	virtual void OnSprintPressed();

	virtual void OnSprintReleased();

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

	virtual void ToggleInventoryPanel();

	virtual void UseInventoryItem();

	virtual void DiscardInventoryItem();

	virtual void PrevInventorySlot();

	virtual void NextInventorySlot();

	virtual void SelectInventorySlot1();
		
	virtual void SelectInventorySlot2();
		
	virtual void SelectInventorySlot3();
		
	virtual void SelectInventorySlot4();
		
	virtual void SelectInventorySlot5();
		
	virtual void SelectInventorySlot6();
		
	virtual void SelectInventorySlot7();
		
	virtual void SelectInventorySlot8();
		
	virtual void SelectInventorySlot9();
		
	virtual void SelectInventorySlot10();

	virtual void PauseOrContinueGame();

	virtual bool RaycastVoxel(FVoxelHitResult& OutHitResult);

public:
	UFUNCTION(BlueprintPure)
	float GetInteractDistance() const { return InteractDistance; }

	UFUNCTION(BlueprintPure)
	bool IsPressedAttack() const { return bPressedAttack; }

	UFUNCTION(BlueprintPure)
	bool IsPressedDefend() const { return bPressedDefend; }

	UFUNCTION(BlueprintPure)
	bool IsPressedSprint() const { return bPressedSprint; }

	UFUNCTION(BlueprintPure)
	float GetCameraTurnRate() const { return CameraTurnRate; }

	UFUNCTION(BlueprintPure)
	float GetCameraLookUpRate() const { return CameraLookUpRate; }

	UFUNCTION(BlueprintPure)
	FVector2D GetCameraDistanceRange() const { return CameraDistanceRange; }

	UFUNCTION(BlueprintPure)
	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintPure)
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }
		
	UFUNCTION(BlueprintPure)
	ADWPlayerCharacterController* GetPlayerController() const;

	UFUNCTION(BlueprintPure)
	ADWPlayerCharacterCameraManager* GetCameraManager() const;
};
