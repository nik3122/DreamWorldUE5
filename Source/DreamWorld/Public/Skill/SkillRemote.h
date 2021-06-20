// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Skill/Skill.h"
#include "SkillRemote.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 * Ô¶³Ì¼¼ÄÜ
 */
UCLASS()
class DREAMWORLD_API ASkillRemote : public ASkill
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillRemote();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FVector InitialVelocity;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Initlize(ADWCharacter* InOwnerCharacter, int32 InSkillIndex) override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USphereComponent* GetSphereComponent() const { return SphereComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
