// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "Skill.generated.h"

class ADWCharacter;
class UStaticMeshComponent;

/**
 * 技能
 */
UCLASS()
class DREAMWORLD_API ASkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkill();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float DurationTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	FName SkillAbilityIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

private:
	FTimerHandle DestroyTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Initlize(ADWCharacter* InOwnerCharacter, const FName& InSkillIndex);

	virtual void Destroyed();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterSkillAbilityData GetSkillData();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetSkillIndex() const { return SkillAbilityIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
};
