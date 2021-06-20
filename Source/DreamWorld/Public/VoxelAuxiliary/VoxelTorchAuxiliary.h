// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "VoxelAuxiliary/VoxelAuxiliary.h"
#include "VoxelTorchAuxiliary.generated.h"

class UVoxel;
class UPointLightComponent;

/**
 * »ð°Ñ¸¨ÖúÎï
 */
UCLASS()
class DREAMWORLD_API AVoxelTorchAuxiliary : public AVoxelAuxiliary
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelTorchAuxiliary();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UPointLightComponent* LightComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(UVoxel* InOwner, FVector InLocaltion) override;
};
