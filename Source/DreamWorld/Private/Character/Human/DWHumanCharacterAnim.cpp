// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Human/DWHumanCharacterAnim.h"
#include "Character/DWCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math.h"
#include "Character/Human/DWHumanCharacter.h"

UDWHumanCharacterAnim::UDWHumanCharacterAnim()
{
	
}

void UDWHumanCharacterAnim::UpdateAnimParams(float DeltaSeconds)
{
	Super::UpdateAnimParams(DeltaSeconds);

	if (!OwnerCharacter) return;

}
