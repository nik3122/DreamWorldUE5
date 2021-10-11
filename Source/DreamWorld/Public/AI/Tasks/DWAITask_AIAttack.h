// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "AI/DWAITaskBase.h"
#include "DWAITask_AIAttack.generated.h"

class ADWCharacter;

/**
 * AI任务_攻击
 */
UCLASS()
class DREAMWORLD_API UDWAITask_AIAttack : public UDWAITaskBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackDistanceKey;

private:
	UPROPERTY()
	ADWCharacter* AttackTarget;

	float AttackDistance;

protected:
	virtual bool InitTask(UBehaviorTreeComponent& OwnerComp) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
