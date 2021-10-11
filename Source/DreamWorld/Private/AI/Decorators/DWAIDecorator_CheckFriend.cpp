// Fill out your copyright notice in the Description page of Project Settings.


#include "DWAIDecorator_CheckFriend.h"
#include "Character/DWCharacter.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AI/DWAIController.h"

UDWAIDecorator_CheckFriend::UDWAIDecorator_CheckFriend(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	
	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIDecorator_CheckFriend, TargetCharacterKey), ADWCharacter::StaticClass());
}

bool UDWAIDecorator_CheckFriend::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ADWAIController* OwnerController = Cast<ADWAIController>(OwnerComp.GetOwner());
	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(OwnerController->GetPawn());
	ADWCharacter* TargetCharacter = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
	if(OwnerCharacter && TargetCharacter)
	{
		return !OwnerCharacter->IsEnemy(TargetCharacter);
	}
	return false;
}
