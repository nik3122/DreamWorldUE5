// Fill out your copyright notice in the Description page of Project Settings.


#include "DWAIDecorator_CheckDistance.h"
#include "Character/DWCharacter.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AI/DWAIController.h"

UDWAIDecorator_CheckDistance::UDWAIDecorator_CheckDistance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	
	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIDecorator_CheckDistance, TargetCharacterKey), ADWCharacter::StaticClass());
	TargetDistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIDecorator_CheckDistance, TargetDistanceKey));
}

bool UDWAIDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ADWAIController* OwnerController = Cast<ADWAIController>(OwnerComp.GetOwner());
	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerController->GetPawn());
	ADWCharacter* TargetCharacter = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
	float TargetDistance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TargetDistanceKey.SelectedKeyName);
	if(OwnerCharacter && TargetCharacter)
	{
		return OwnerCharacter->Distance(TargetCharacter, false, false) <= TargetDistance;
	}
	return false;
}
