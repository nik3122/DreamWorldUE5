// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Vitality/VitalityObject.h"
#include "VitalityVoxel.generated.h"

class UVoxelMeshComponent;

/**
 * 有生命的体素
 */
UCLASS()
class DREAMWORLD_API AVitalityVoxel : public AVitalityObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVitalityVoxel();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FName VoxelID;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	UVoxelMeshComponent* VoxelMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVoxelData GetVoxelData();
};
