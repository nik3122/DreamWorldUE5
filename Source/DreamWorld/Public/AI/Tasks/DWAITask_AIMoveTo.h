// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/DWAITaskBase.h"
#include "DWAITask_AIMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class DREAMWORLD_API UDWAITask_AIMoveTo : public UDWAITaskBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float DurationTime;

private:
	UPROPERTY()
	ADWCharacter* TargetCharacter;

	FVector TargetLocation;

	float TargetDistance;

	float LocalRemainTime;

protected:
	virtual bool InitTask(UBehaviorTreeComponent& OwnerComp) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
