// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Character/DWCharacter.h"
#include "DWMonsterCharacter.generated.h"

class UBoxComponent;

/**
 * ¹ÖÎï½ÇÉ«
 */
UCLASS()
class DREAMWORLD_API ADWMonsterCharacter : public ADWCharacter
{
	GENERATED_BODY()
	
public:
	ADWMonsterCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
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
};
