// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DWAIDecoratorBase.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "AI/DWAIController.h"
#include "Character/DWCharacter.h"

UDWAIDecoratorBase::UDWAIDecoratorBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

bool UDWAIDecoratorBase::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
}
