// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Gameplay/WHPlayerController.h"

#include "DWPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSpawned, class ADWPlayerCharacter*, InPlayerCharacter);

/**
 * 玩家角色控制器
 */
UCLASS()
class DREAMWORLD_API ADWPlayerController : public AWHPlayerController
{
	GENERATED_BODY()

public:
	ADWPlayerController();
	
protected:
	static class UPlayerDataSave* DataSave;

public:
	static UPlayerDataSave* GetDataSave() { return DataSave; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraLookUpRate;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	FVector2D CameraDistanceRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ADWPlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inputs")
	bool bPressedSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inputs")
	float DoubleJumpTime;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerSpawned OnPlayerSpawned;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	virtual void OnBasicGenerated(FVector InPlayerLocation);

protected:
	virtual void TurnCam(float InRate);

	virtual void LookUpCam(float InRate);
	
	virtual void ZoomNear();
	
	virtual void ZoomFar();

	virtual void MoveForward(float InValue);

	virtual void MoveRight(float InValue);

	virtual void MoveUp(float InValue);

	virtual void OnJumpPressed();

	virtual void OnJumpReleased();

	virtual void OnSprintPressed();

	virtual void OnSprintReleased();

	virtual void ToggleInventoryPanel();

	virtual void UseInventoryItem();

	virtual void UseAllInventoryItem();

	virtual void DiscardInventoryItem();

	virtual void DiscardAllInventoryItem();

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

public:
	virtual void Tick(float DeltaTime) override;

	void LoadPlayer(const FString& InPlayerName);
	
	void UnLoadPlayer();

	void ResetData();

	bool RaycastFromAimPoint(FHitResult& OutHitResult, EGameTraceType InGameTraceType, float InRayDistance = 1000) const;

public:
	UFUNCTION(BlueprintPure)
	float GetCameraTurnRate() const { return CameraTurnRate; }

	UFUNCTION(BlueprintPure)
	float GetCameraLookUpRate() const { return CameraLookUpRate; }

	UFUNCTION(BlueprintPure)
	FVector2D GetCameraDistanceRange() const { return CameraDistanceRange; }

	UFUNCTION(BlueprintPure)
	class ADWPlayerCharacter* GetPlayerCharacter() const {return PlayerCharacter; }

	UFUNCTION(BlueprintPure)
	class ADWCharacter* GetProcessedCharacter() const;

	UFUNCTION(BlueprintPure)
	class ADWPlayerCharacterCameraManager* GetCameraManager() const;
};
