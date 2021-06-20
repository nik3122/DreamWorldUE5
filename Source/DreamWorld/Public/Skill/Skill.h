// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "Skill.generated.h"

class ADWCharacter;
class UStaticMeshComponent;

/**
 * ¼¼ÄÜ
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
	int32 SkillAbilityIndex;

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
	virtual void Initlize(ADWCharacter* InOwnerCharacter, int32 InSkillIndex);

	virtual void Destroyed();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDWCharacterSkillAbilityData GetSkillData();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSkillIndex() const { return SkillAbilityIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
};
