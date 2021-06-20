// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/DWPlayerCharacterAnim.h"
#include "Character/Player/DWPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math.h"

UDWPlayerCharacterAnim::UDWPlayerCharacterAnim()
{
}

void UDWPlayerCharacterAnim::UpdateAnimParams(float DeltaSeconds)
{
	Super::UpdateAnimParams(DeltaSeconds);

	if (!OwnerCharacter) return;

}
