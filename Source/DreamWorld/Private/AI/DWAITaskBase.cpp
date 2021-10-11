// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DWAITaskBase.h"
#include "AI/DWAIController.h"
#include "Character/DWCharacter.h"

UDWAITaskBase::UDWAITaskBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OwnerController = nullptr;
	OwnerCharacter = nullptr;
}

bool UDWAITaskBase::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	OwnerController = Cast<ADWAIController>(OwnerComp.GetOwner());
	if (OwnerController)
	{
		OwnerCharacter = Cast<ADWCharacter>(OwnerController->GetPawn());
	}
	return OwnerController && OwnerCharacter;
}

void UDWAITaskBase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitTask(OwnerComp)) return;

}

EBTNodeResult::Type UDWAITaskBase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITaskBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	return EBTNodeResult::Succeeded;
}

void UDWAITaskBase::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(!InitTask(OwnerComp)) return;

}
