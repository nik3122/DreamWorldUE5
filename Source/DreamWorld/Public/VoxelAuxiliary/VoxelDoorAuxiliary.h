// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "VoxelAuxiliary/VoxelVirtualAuxiliary.h"
#include "VoxelDoorAuxiliary.generated.h"

class UVoxel;

/**
 * �Ÿ�����
 */
UCLASS()
class DREAMWORLD_API AVoxelDoorAuxiliary : public AVoxelVirtualAuxiliary
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AVoxelDoorAuxiliary();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Initialize(UVoxel* InOwner, FVector InLocation) override;
};
