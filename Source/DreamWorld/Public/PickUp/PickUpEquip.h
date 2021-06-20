// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "PickUp/PickUp.h"
#include "PickUpEquip.generated.h"

/**
 * 可拾取装备
 */
UCLASS()
class DREAMWORLD_API APickUpEquip : public APickUp
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickUpEquip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPickUp(ADWCharacter* InPicker) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(FItem InItem, bool bPreview = false) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEquipData GetEquipData();
};
