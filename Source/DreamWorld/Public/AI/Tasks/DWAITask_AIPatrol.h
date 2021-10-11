// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "AI/DWAITaskBase.h"
#include "DWAITask_AIPatrol.generated.h"

/**
 * AI任务_巡逻
 */
UCLASS()
class DREAMWORLD_API UDWAITask_AIPatrol : public UDWAITaskBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolDistanceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolDurationKey;

private:
	float PatrolDistance;

	float PatrolDuration;

	FVector PatrolLocation;
	
protected:
	virtual bool InitTask(UBehaviorTreeComponent& OwnerComp) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
