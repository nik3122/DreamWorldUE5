// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Equip/Equip.h"
#include "EquipShield.generated.h"

/**
 * ∂‹≈∆
 */
UCLASS()
class DREAMWORLD_API AEquipShield : public AEquip
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquipShield();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float DefendRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterStats")
	float DefendScope;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEquipShieldData GetShieldData();
};
