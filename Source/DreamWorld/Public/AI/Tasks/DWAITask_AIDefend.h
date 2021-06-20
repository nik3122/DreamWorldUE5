// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "AI/DWAITaskBase.h"
#include "DWAITask_AIDefend.generated.h"

/**
 * AIÈÎÎñ_·ÀÓù
 */
UCLASS()
class DREAMWORLD_API UDWAITask_AIDefend : public UDWAITaskBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DefendTargetKey;

private:
	float AIMoveRemainTime;

	FVector AIMoveLocation;
	
	ADWCharacter* DefendTarget;

protected:
	virtual bool InitTask(UBehaviorTreeComponent& OwnerComp) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
