// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "VoxelAuxiliary/VoxelAuxiliary.h"
#include "VoxelEntityAuxiliary.generated.h"

class UVoxel;
class UBoxComponent;

/**
 * ʵ�帨����
 */
UCLASS()
class DREAMWORLD_API AVoxelEntityAuxiliary : public AVoxelAuxiliary
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AVoxelEntityAuxiliary();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UBoxComponent* BoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(UVoxel* InOwner, FVector InLocation) override;
};
