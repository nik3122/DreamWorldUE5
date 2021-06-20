// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "DWAIController.generated.h"

class ADWCharacter;
class UBehaviorTreeComponent;

DECLARE_DELEGATE_TwoParams(FDWAIPerceptionUpdated, ADWCharacter*, bool)

/**
 * AI¿ØÖÆÆ÷»ùÀà
 */
UCLASS()
class DREAMWORLD_API ADWAIController : public AAIController
{
    GENERATED_BODY()

public:
    ADWAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

private:
	bool bLostPerceptionTarget;

	float RedirectRemainTime;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECharacterNature GetCharacterNature() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterNature(ECharacterNature InCharacterNature);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackDistance() const;

	UFUNCTION(BlueprintCallable)
	void SetAttackDistance(float InAttackDistance);
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetFollowDistance() const;

	UFUNCTION(BlueprintCallable)
	void SetFollowDistance(float InFollowDistance);
			
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPatrolDistance() const;

	UFUNCTION(BlueprintCallable)
	void SetPatrolDistance(float InPatrolDistance);
				
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPatrolDuration() const;

	UFUNCTION(BlueprintCallable)
	void SetPatrolDuration(float InPatrolDuration);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWCharacter* GetTargetCharacter() const;
	
	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter(ADWCharacter* InTargetCharacter);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsLostTarget() const;
	
	UFUNCTION(BlueprintCallable)
	void SetIsLostTarget(bool bLostTarget, FVector InLostTargetLocation = FVector::ZeroVector);
};
