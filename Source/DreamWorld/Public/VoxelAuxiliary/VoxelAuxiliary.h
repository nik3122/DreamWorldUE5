// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "VoxelAuxiliary.generated.h"

class UVoxel;

/**
 * ÌåËØ¸¨ÖúÎï
 */
UCLASS()
class DREAMWORLD_API AVoxelAuxiliary : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelAuxiliary();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVoxel* OwnerVoxel;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(UVoxel* InOwner, FVector InLocaltion);
};
