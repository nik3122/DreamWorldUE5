// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Vitality/VitalityVoxel.h"
#include "VitalityPlant.generated.h"

class UVoxelMeshComponent;

/**
 * ��������ֲ��
 */
UCLASS()
class DREAMWORLD_API AVitalityPlant : public AVitalityVoxel
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVitalityPlant();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};