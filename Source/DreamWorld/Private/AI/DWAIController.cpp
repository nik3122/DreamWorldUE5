// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DWAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/DWCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/DWAIBlackboard.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

ADWAIController::ADWAIController()
{
	bAttachToPawn = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ADWAIController::OnTargetPerceptionUpdated);

	FAISenseAffiliationFilter affiliationFilter;
	affiliationFilter.bDetectEnemies = true;
	affiliationFilter.bDetectFriendlies = true;
	affiliationFilter.bDetectNeutrals = true;

	auto sightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	sightSenseConfig->SightRadius = 1000;
	sightSenseConfig->LoseSightRadius = 1200;
	sightSenseConfig->PeripheralVisionAngleDegrees = 90;
	sightSenseConfig->DetectionByAffiliation = affiliationFilter;
	sightSenseConfig->SetMaxAge(5);
	AIPerception->ConfigureSense(*sightSenseConfig);
	AIPerception->SetDominantSense(*sightSenseConfig->GetSenseImplementation());

	const auto damageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	AIPerception->ConfigureSense(*damageSenseConfig);

	bLostPerceptionTarget = false;
	RedirectRemainTime = 0;
}

void ADWAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (GetPossessedCharacter())
	{
		GetPossessedCharacter()->SetOwnerController(this);
		GetPossessedCharacter()->RefreshData();
		if (GetPossessedCharacter()->HasBehaviorTree())
		{
			RunBehaviorTree(GetPossessedCharacter()->GetBehaviorTree());

			SetCharacterNature(GetPossessedCharacter()->GetNature());
			SetAttackDistance(GetPossessedCharacter()->GetAttackDistance());
			SetFollowDistance(GetPossessedCharacter()->GetFollowDistance());
			SetPatrolDistance(GetPossessedCharacter()->GetPatrolDistance());
			SetPatrolDuration(GetPossessedCharacter()->GetPatrolDuration());
			SetTargetCharacter(nullptr);
			SetLostTarget(false);
		}
	}
}

void ADWAIController::OnUnPossess()
{
	if(GetPossessedCharacter())
	{
		GetPossessedCharacter()->SetOwnerController(nullptr);
	}

	Super::OnUnPossess();

	if (GetBrainComponent() && GetBrainComponent()->IsA(UBehaviorTreeComponent::StaticClass()))
	{
		Cast<UBehaviorTreeComponent>(GetBrainComponent())->StopTree();
	}
}

void ADWAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADWAIController::OnTargetCharacterDead()
{
	SetTargetCharacter(nullptr);
	SetLostTarget(false);
}

void ADWAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!GetPossessedCharacter()) return;
	
	ADWCharacter* character = Cast<ADWCharacter>(Actor);
	
	if (character && character != GetPossessedCharacter() && !character->IsDead())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (!GetTargetCharacter())
			{
				SetTargetCharacter(character);
				SetLostTarget(false);
			}
			else if(character == GetTargetCharacter())
			{
				SetLostTarget(false);
			}
		}
		else if(character == GetTargetCharacter())
		{
			SetLostTarget(true, character->GetActorLocation());
		}
	}
}

ECharacterNature ADWAIController::GetCharacterNature() const
{
	if(!GetBlackboardComponent()) return ECharacterNature::AIFriendly;

	return (ECharacterNature)GetBlackboardComponent()->GetValueAsEnum(FName("CharacterNature"));
}

void ADWAIController::SetCharacterNature(ECharacterNature InCharacterNature)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsEnum(FName("CharacterNature"), (uint8)InCharacterNature);
}

float ADWAIController::GetAttackDistance() const
{
	if(!GetBlackboardComponent()) return 0.f;

	return GetBlackboardComponent()->GetValueAsFloat(FName("AttackDistance"));
}

void ADWAIController::SetAttackDistance(float InAttackDistance)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsFloat(FName("AttackDistance"), InAttackDistance);
}

float ADWAIController::GetFollowDistance() const
{
	if(!GetBlackboardComponent()) return 0.f;

	return GetBlackboardComponent()->GetValueAsFloat(FName("FollowDistance"));
}

void ADWAIController::SetFollowDistance(float InFollowDistance)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsFloat(FName("FollowDistance"), InFollowDistance);
}

float ADWAIController::GetPatrolDistance() const
{
	if(!GetBlackboardComponent()) return 0.f;

	return GetBlackboardComponent()->GetValueAsFloat(FName("PatrolDistance"));
}

void ADWAIController::SetPatrolDistance(float InPatrolDistance)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsFloat(FName("PatrolDistance"), InPatrolDistance);
}

float ADWAIController::GetPatrolDuration() const
{
	if(!GetBlackboardComponent()) return 0.f;

	return GetBlackboardComponent()->GetValueAsFloat(FName("PatrolDuration"));
}

void ADWAIController::SetPatrolDuration(float InPatrolDuration)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsFloat(FName("PatrolDuration"), InPatrolDuration);
}

bool ADWAIController::IsLostTarget() const
{
	if(!GetBlackboardComponent()) return false;

	return GetBlackboardComponent()->GetValueAsBool(FName("IsLostTarget"));
}

void ADWAIController::SetLostTarget(bool bLostTarget, FVector InLostTargetLocation /*= FVector::ZeroVector*/)
{
	if(!GetBlackboardComponent()) return;

	GetBlackboardComponent()->SetValueAsBool(FName("IsLostTarget"), bLostTarget);
	GetBlackboardComponent()->SetValueAsVector(FName("LostTargetLocation"), InLostTargetLocation);
}

ADWCharacter* ADWAIController::GetTargetCharacter() const
{
	if(!GetBlackboardComponent()) return nullptr;
	
	return Cast<ADWCharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetCharacter")));
}

void ADWAIController::SetTargetCharacter(ADWCharacter* InTargetCharacter)
{
	if(!GetBlackboardComponent()) return;

	if(GetTargetCharacter())
	{
		GetTargetCharacter()->OnCharacterDead.RemoveDynamic(this, &ADWAIController::OnTargetCharacterDead);
	}
	if(InTargetCharacter && !InTargetCharacter->OnCharacterDead.Contains(this, FName("OnTargetCharacterDead")))
	{
		InTargetCharacter->OnCharacterDead.AddDynamic(this, &ADWAIController::OnTargetCharacterDead);
	}

	GetBlackboardComponent()->SetValueAsObject(FName("TargetCharacter"), InTargetCharacter);
}

ADWCharacter* ADWAIController::GetPossessedCharacter() const
{
	return Cast<ADWCharacter>(GetPawn());
}
