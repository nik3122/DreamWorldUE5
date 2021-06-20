// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/DWAIService_RefreshState.h"
#include "Character/DWCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/DWAIController.h"

UDWAIService_RefreshState::UDWAIService_RefreshState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CharacterAIStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIService_RefreshState, CharacterAIStateKey.SelectedKeyName), FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterAIState"), true));
	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIService_RefreshState, TargetCharacterKey.SelectedKeyName), ADWCharacter::StaticClass());
	IsLostTargetKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UDWAIService_RefreshState, IsLostTargetKey.SelectedKeyName));

	CharacterAIState = ECharacterAIState::None;
	TargetCharacter = nullptr;
}

bool UDWAIService_RefreshState::InitService(UBehaviorTreeComponent& OwnerComp)
{
	return Super::InitService(OwnerComp);
}

void UDWAIService_RefreshState::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitService(OwnerComp)) return;

	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	SetCharacterAIState(OwnerComp, ECharacterAIState::None);
}

void UDWAIService_RefreshState::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!InitService(OwnerComp)) return;

	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UDWAIService_RefreshState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!InitService(OwnerComp) || !OwnerCharacter->IsActive()) return;

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	TargetCharacter = Cast<ADWCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
	IsLostTarget = OwnerComp.GetBlackboardComponent()->GetValueAsBool(IsLostTargetKey.SelectedKeyName);

	if (TargetCharacter)
	{
		if (!IsLostTarget)
		{
			switch (OwnerCharacter->GetNature())
			{
				case ECharacterNature::AIFriendly:
				{
					SetCharacterAIState(OwnerComp, ECharacterAIState::AIFollow);
					break;
				}
				case ECharacterNature::AIHostile:
				{
					if (OwnerCharacter->ValidInteract(TargetCharacter, ECharacterInteract::Fighting))
					{
						//if (OwnerCharacter->Distance(TargetCharacter, false, false) >= OwnerCharacter->GetAIFollowDistance())
						//{
						//	SetCharacterAIState(OwnerComp, ECharacterAIState::AIFollow);
						//}
						//else
						//{
						//	//if (!TargetCharacter->IsAttacking())
						//	//{
						//		SetCharacterAIState(OwnerComp, ECharacterAIState::AIAttack);
						//	//}
						//	//else
						//	//{
						//	//	SetCharacterAIState(OwnerComp, ECharacterAIState::AIDefend);
						//	//}
						//}
					}
					break;
				}
			}
		}
		else
		{
			SetCharacterAIState(OwnerComp, ECharacterAIState::AITrack);
		}
		if (TargetCharacter->IsDead())
		{
			SetTargetCharacter(OwnerComp, nullptr);
			SetIsLostTarget(OwnerComp, false);
		}
	}
	else
	{
		//SetCharacterAIState(OwnerComp, OwnerCharacter->IsAllowAIPatrol() ? ECharacterAIState::AIPatrol : ECharacterAIState::None);
	}
}

void UDWAIService_RefreshState::SetCharacterAIState(UBehaviorTreeComponent& OwnerComp, ECharacterAIState InAIState)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(CharacterAIStateKey.SelectedKeyName, (uint8)InAIState);
}

void UDWAIService_RefreshState::SetTargetCharacter(UBehaviorTreeComponent& OwnerComp, ADWCharacter* InTargetCharacter)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetCharacterKey.SelectedKeyName, InTargetCharacter);
}

void UDWAIService_RefreshState::SetIsLostTarget(UBehaviorTreeComponent& OwnerComp, bool InIsLostTarget)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsLostTargetKey.SelectedKeyName, InIsLostTarget);
}
