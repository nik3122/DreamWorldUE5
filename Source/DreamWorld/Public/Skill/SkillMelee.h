// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Skill/Skill.h"
#include "SkillMelee.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 * 近战技能
 */
UCLASS()
class DREAMWORLD_API ASkillMelee : public ASkill
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillMelee();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
