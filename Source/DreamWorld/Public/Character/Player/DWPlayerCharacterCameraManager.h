// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Camera/PlayerCameraManager.h"
#include "DWPlayerCharacterCameraManager.generated.h"

class ADWPlayerCharacter;
class USpringArmComponent;

/**
 * ��������������
 */
UCLASS()
class DREAMWORLD_API ADWPlayerCharacterCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ADWPlayerCharacterCameraManager();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	float MinCameraDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	float MaxCameraDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	float InitCameraDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	float CameraZoomSpeed;

private:
	float TargetCameraDistance;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintPure)
	float GetCameraDistance() const;

	UFUNCTION(BlueprintCallable)
	void SetCameraDistance(float InCameraDistance = -1.f, bool bInstant = false);

	UFUNCTION(BlueprintCallable)
	void ZoomCamera(float InDeltaValue);

	UFUNCTION(BlueprintPure)
	USpringArmComponent* GetCameraBoom() const;
};
