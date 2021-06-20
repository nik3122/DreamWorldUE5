// Fill out your copyright notice in the Description page of Project Settings.


#include "DWAITask_AIMoveTo.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UDWAITask_AIMoveTo::UDWAITask_AIMoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;

	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIMoveTo, TargetKey), ADWCharacter::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIMoveTo, TargetKey));
	DistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIMoveTo, DistanceKey));

	TargetCharacter = nullptr;
	TargetLocation = FVector();
	TargetDistance = 0.f;
	DurationTime = -1.f;
	LocalRemainTime = 0.f;
}

bool UDWAITask_AIMoveTo::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	if (!Super::InitTask(OwnerComp)) return false;

	if (TargetKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
	}
	else if (TargetCharacter == nullptr)
	{
		TargetCharacter = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
		return TargetCharacter && TargetCharacter->IsValidLowLevel();
	}
	TargetDistance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(DistanceKey.SelectedKeyName);
	return true;
}

void UDWAITask_AIMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(!InitTask(OwnerComp)) return;

	if (TargetKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		if(OwnerCharacter->DoAIMove(TargetLocation, TargetDistance))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		if(OwnerCharacter->DoAIMove(TargetCharacter, TargetDistance))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	if(LocalRemainTime != -1.f)
	{
		LocalRemainTime -= DeltaSeconds;
		if(LocalRemainTime <= 0.f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

EBTNodeResult::Type UDWAITask_AIMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;
	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_AIMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	LocalRemainTime = DurationTime;
	OwnerCharacter->SetMotionRate(0.7f, 0.7f);
	OwnerCharacter->SetLockedTarget(nullptr);
	
	return EBTNodeResult::InProgress;
}
