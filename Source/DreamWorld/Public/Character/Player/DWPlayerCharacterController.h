// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/PlayerController.h"
#include "DWPlayerCharacterController.generated.h"

class ADWPlayerCharacter;

/**
 * 玩家角色控制器
 */
UCLASS()
class DREAMWORLD_API ADWPlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ADWPlayerCharacterController();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWPlayerCharacter* PossessedCharacter;

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWPlayerCharacter* GetPossessedCharacter() const { return PossessedCharacter; };

	bool RaycastFromAimPoint(FHitResult& OutHitResult, EGameTraceType InGameTraceType, float InRayDistance = 1000);
};
