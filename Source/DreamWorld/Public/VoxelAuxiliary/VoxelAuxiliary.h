// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Interaction.h"
#include "GameFramework/Actor.h"
#include "VoxelAuxiliary.generated.h"

class UVoxel;

/**
 * ���ظ�����
 */
UCLASS()
class DREAMWORLD_API AVoxelAuxiliary : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelAuxiliary();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Initialize(AChunk* InOwnerChunk, FIndex InVoxelIndex);

	virtual bool OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractionComponent* Interaction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIndex VoxelIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AChunk* OwnerChunk;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<EInteractOption> InteractOptions;

	IInteraction* InteractingTarget;

public:
	FIndex GetVoxelIndex() const { return VoxelIndex; }

	FVoxelItem& GetVoxelItem() const;

	virtual TArray<EInteractOption> GetInteractOptions(IInteraction* InTrigger) const override { return InteractOptions; }

	virtual IInteraction* GetInteractingTarget() const override { return InteractingTarget; }

	virtual void SetInteractingTarget(IInteraction* InTarget) override { InteractingTarget = InTarget; }
};
