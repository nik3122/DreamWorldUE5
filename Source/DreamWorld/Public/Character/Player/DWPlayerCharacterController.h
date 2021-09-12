// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/PlayerController.h"
#include "DWPlayerCharacterController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSpawned, ADWPlayerCharacter*, InPlayerCharacter);

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
	static class UPlayerDataSave* DataSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ADWPlayerCharacter* PossessedCharacter;

public:
	static UPlayerDataSave* GetDataSave() { return DataSave; }

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSpawned OnPlayerSpawned;

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	virtual void OnBasicGenerated(FVector InPlayerLocation);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void LoadPlayer(const FString& InPlayerName);
	
	virtual void UnLoadPlayer();

	UFUNCTION(BlueprintPure)
	class ADWPlayerCharacter* GetPossessedCharacter() const {return PossessedCharacter; }

	bool RaycastFromAimPoint(FHitResult& OutHitResult, EGameTraceType InGameTraceType, float InRayDistance = 1000);
};
