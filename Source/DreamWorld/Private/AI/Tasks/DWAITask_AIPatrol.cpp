// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/DWAITask_AIPatrol.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

UDWAITask_AIPatrol::UDWAITask_AIPatrol(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;

	PatrolDistance = 0.f;
	PatrolDuration = 0.f;
	PatrolLocation = FVector();

	PatrolDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIPatrol, PatrolDistanceKey));
	PatrolDurationKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIPatrol, PatrolDurationKey));
}

bool UDWAITask_AIPatrol::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	return Super::InitTask(OwnerComp);
}

void UDWAITask_AIPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitTask(OwnerComp)) return;

	PatrolDuration -= DeltaSeconds;
	if (PatrolDuration <= 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (OwnerCharacter->DoAIMove(PatrolLocation))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UDWAITask_AIPatrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_AIPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->SetMotionRate(0.3f, 0.5f);

	PatrolDistance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(PatrolDistanceKey.SelectedKeyName);
	PatrolDuration= OwnerComp.GetBlackboardComponent()->GetValueAsFloat(PatrolDurationKey.SelectedKeyName);

	PatrolDuration = FMath::RandRange(PatrolDuration - 2, PatrolDuration + 2);
	PatrolLocation = OwnerCharacter->GetBirthLocation() + FRotator(0, FMath::RandRange(0, 360), 0).Vector() * FMath::FRandRange(0, PatrolDistance);

	return EBTNodeResult::InProgress;
}
