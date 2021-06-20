// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Character/DWCharacter.h"
#include "DWHumanCharacter.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AEquipWeapon;
class AEquipShield;
class AEquipArmor;
class UInventoryEquipSlot;

/**
 * 类人角色
 */
UCLASS()
class DREAMWORLD_API ADWHumanCharacter : public ADWCharacter
{
	GENERATED_BODY()

public:
	ADWHumanCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetDamaging(bool bInDamaging) override;

	virtual bool Attack(int32 InAbilityIndex = -1) override;

	virtual bool SkillAttack(int32 InAbilityIndex = -1) override;

	virtual void Defend() override;
};
