// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DWAITaskBase.generated.h"

class ADWAIController;
class ADWCharacter;

/**
 * AI任务基类
 */
UCLASS()
class DREAMWORLD_API UDWAITaskBase : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(Transient)
	ADWAIController* OwnerController;

	UPROPERTY(Transient)
	ADWCharacter* OwnerCharacter;
	
protected:
	virtual bool InitTask(UBehaviorTreeComponent& OwnerComp);

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
