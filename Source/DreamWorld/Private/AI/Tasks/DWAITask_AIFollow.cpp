// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/DWAITask_AIFollow.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

UDWAITask_AIFollow::UDWAITask_AIFollow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	
	TargetCharacter = nullptr;
	TargetDistance = 0.f;

	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIFollow, TargetCharacterKey), ADWCharacter::StaticClass());
	TargetDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIFollow, TargetDistanceKey));
}

bool UDWAITask_AIFollow::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	if(!Super::InitTask(OwnerComp)) return false;

	if (TargetCharacter == nullptr)
	{
		TargetCharacter = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
		TargetDistance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TargetDistanceKey.SelectedKeyName);
	}
	return TargetCharacter && TargetCharacter->IsValidLowLevel();
}

void UDWAITask_AIFollow::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitTask(OwnerComp)) return;

	OwnerCharacter->DoAIMove(TargetCharacter, TargetDistance, true);
}

EBTNodeResult::Type UDWAITask_AIFollow::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->SetLockedTarget(nullptr);

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_AIFollow::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->SetMotionRate(1, 1);

	return EBTNodeResult::InProgress;
}
