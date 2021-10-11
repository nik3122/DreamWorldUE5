// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DWAIBlackboard.h"
#include "Character/DWCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

void UDWAIBlackboard::PostLoad()
{
	Super::PostLoad();

	// FBlackboardEntry CharacterNature;
	// CharacterNature.EntryName = FName("CharacterNature");
	// UBlackboardKeyType_Enum* CharacterNatureKeyType = NewObject<UBlackboardKeyType_Enum>();
	// CharacterNatureKeyType->EnumType = FindObject<UEnum>(ANY_PACKAGE, *FString("ECharacterNature"), true);
	// CharacterNatureKeyType->EnumName = TEXT("ECharacterNature");
	// CharacterNature.KeyType = CharacterNatureKeyType;
	// Keys.Add(CharacterNature);
	//
	// FBlackboardEntry AttackDistance;
	// AttackDistance.EntryName = FName("AttackDistance");
	// UBlackboardKeyType_Float* AttackDistanceKeyType = NewObject<UBlackboardKeyType_Float>();
	// AttackDistance.KeyType = AttackDistanceKeyType;
	// Keys.Add(AttackDistance);
	//
	// FBlackboardEntry FollowDistance;
	// FollowDistance.EntryName = FName("FollowDistance");
	// UBlackboardKeyType_Float* FollowDistanceKeyType = NewObject<UBlackboardKeyType_Float>();
	// FollowDistance.KeyType = FollowDistanceKeyType;
	// Keys.Add(FollowDistance);
	// 	
	// FBlackboardEntry PatrolDistance;
	// PatrolDistance.EntryName = FName("PatrolDistance");
	// UBlackboardKeyType_Float* PatrolDistanceKeyType = NewObject<UBlackboardKeyType_Float>();
	// PatrolDistance.KeyType = PatrolDistanceKeyType;
	// Keys.Add(PatrolDistance);
	// 		
	// FBlackboardEntry PatrolDuration;
	// PatrolDuration.EntryName = FName("PatrolDuration");
	// UBlackboardKeyType_Float* PatrolDurationKeyType = NewObject<UBlackboardKeyType_Float>();
	// PatrolDuration.KeyType = PatrolDurationKeyType;
	// Keys.Add(PatrolDuration);
	//
	// FBlackboardEntry TargetCharacter;
	// TargetCharacter.EntryName = FName("TargetCharacter");
	// UBlackboardKeyType_Object* TargetCharacterKeyType = NewObject<UBlackboardKeyType_Object>();
	// TargetCharacterKeyType->BaseClass = ADWCharacter::StaticClass();
	// TargetCharacter.KeyType = TargetCharacterKeyType;
	// Keys.Add(TargetCharacter);
	//
	// FBlackboardEntry IsLostTarget;
	// IsLostTarget.EntryName = FName("IsLostTarget");
	// UBlackboardKeyType_Bool* IsLostTargetKeyType = NewObject<UBlackboardKeyType_Bool>();
	// IsLostTarget.KeyType = IsLostTargetKeyType;
	// Keys.Add(IsLostTarget);
	//
	// FBlackboardEntry LostTargetLocation;
	// LostTargetLocation.EntryName = FName("LostTargetLocation");
	// UBlackboardKeyType_Vector* LostTargetLocationKeyType = NewObject<UBlackboardKeyType_Vector>();
	// LostTargetLocation.KeyType = LostTargetLocationKeyType;
	// Keys.Add(LostTargetLocation);
}
