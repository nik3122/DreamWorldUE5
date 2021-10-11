// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Character/DWCharacter.h"
#include "DWMonsterCharacter.generated.h"

class UBoxComponent;

/**
 * �����ɫ
 */
UCLASS()
class DREAMWORLD_API ADWMonsterCharacter : public ADWCharacter
{
	GENERATED_BODY()
	
public:
	ADWMonsterCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* AttackPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetDamaging(bool bInDamaging) override;

	virtual bool OnInteract(IInteraction* InTrigger, EInteractOption InInteractOption) override;

	virtual TArray<EInteractOption> GetInteractOptions(IInteraction* InTrigger) const override;
};
