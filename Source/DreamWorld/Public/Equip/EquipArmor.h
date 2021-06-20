// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Equip/Equip.h"
#include "EquipArmor.generated.h"

/**
 * ·À¾ß
 */
UCLASS()
class DREAMWORLD_API AEquipArmor : public AEquip
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipArmor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEquipArmorData GetArmorData();
};
