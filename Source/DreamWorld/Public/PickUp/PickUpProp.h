// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "PickUp/PickUp.h"
#include "PickUpProp.generated.h"

/**
 * 可拾取道具
 */
UCLASS()
class DREAMWORLD_API APickUpProp : public APickUp
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickUpProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnPickUp(ADWCharacter* InPicker) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(FItem InItem, bool bPreview = false) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPropData GetPropData();
};
