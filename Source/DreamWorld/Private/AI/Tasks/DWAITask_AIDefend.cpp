// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/DWAITask_AIDefend.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/DWCharacter.h"
#include "AI/DWAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

UDWAITask_AIDefend::UDWAITask_AIDefend(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;

	DefendTarget = nullptr;
	DefendTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAITask_AIDefend, DefendTargetKey), ADWCharacter::StaticClass());
	AIMoveRemainTime = FMath::FRandRange(1, 2);
	AIMoveLocation = FVector::ZeroVector;
}

bool UDWAITask_AIDefend::InitTask(UBehaviorTreeComponent& OwnerComp)
{
	if(!Super::InitTask(OwnerComp)) return false;

	DefendTarget = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DefendTargetKey.SelectedKeyName));
	return DefendTarget && DefendTarget->IsValidLowLevel();
}

void UDWAITask_AIDefend::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitTask(OwnerComp)) return;

	AIMoveRemainTime -= DeltaSeconds;
	if (AIMoveRemainTime <= 0)
	{
		//AIMoveLocation = OwnerCharacter->GetPatrolLocation(AIMoveRemainTime);
	}
	if (OwnerCharacter->DoAIMove(AIMoveLocation))
	{
		AIMoveRemainTime = FMath::FRandRange(1, 2);
	}
}

EBTNodeResult::Type UDWAITask_AIDefend::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->UnDefend();
	OwnerCharacter->SetLockedTarget(nullptr);

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UDWAITask_AIDefend::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitTask(OwnerComp)) return EBTNodeResult::Failed;

	OwnerCharacter->Defend();
	OwnerCharacter->SetLockedTarget(DefendTarget);

	//OwnerCharacter->SetMotionRate(1, 1);

	return EBTNodeResult::InProgress;
}
