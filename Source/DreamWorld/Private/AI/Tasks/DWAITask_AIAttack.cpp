// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/DWAITask_AIAttack.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UDWAITask_AIAttack::UDWAITask_AIAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	
	AttackTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIAttack, AttackTargetKey), ADWCharacter::StaticClass());
	AttackDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIAttack, AttackDistanceKey));

	AttackTarget = nullptr;
	AttackDistance = 0.f;
}

bool UDWAITask_AIAttack::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	if(!Super::InitTask(OwnerComp)) return false;

	AttackTarget = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	AttackDistance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackDistanceKey.SelectedKeyName);

	return AttackTarget && AttackTarget->IsValidLowLevel();
}

void UDWAITask_AIAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitTask(OwnerComp)) return;

	if(OwnerCharacter->Distance(AttackTarget, false, false) <= AttackDistance)
	{
		OwnerCharacter->Attack();
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UDWAITask_AIAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->UnAttack();
	OwnerCharacter->SetLockedTarget(nullptr);

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_AIAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->SetLockedTarget(AttackTarget);

	return EBTNodeResult::InProgress;
}

void UDWAITask_AIAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	OwnerCharacter->UnAttack();
	OwnerCharacter->SetLockedTarget(nullptr);
}
