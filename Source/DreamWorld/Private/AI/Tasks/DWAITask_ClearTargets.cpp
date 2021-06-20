// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/DWAITask_ClearTargets.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UDWAITask_ClearTargets::UDWAITask_ClearTargets(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	
	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_ClearTargets, TargetCharacterKey), ADWCharacter::StaticClass());
	IsLostTargetKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_ClearTargets, IsLostTargetKey));
	LostTargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_ClearTargets, LostTargetLocationKey));
}

EBTNodeResult::Type UDWAITask_ClearTargets::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_ClearTargets::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetCharacterKey.SelectedKeyName, nullptr);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsLostTargetKey.SelectedKeyName, false);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(LostTargetLocationKey.SelectedKeyName, FVector());
	
	return EBTNodeResult::Succeeded;
}
