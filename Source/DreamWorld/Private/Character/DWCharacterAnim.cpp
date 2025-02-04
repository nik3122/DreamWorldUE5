// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DWCharacterAnim.h"
#include "Character/DWCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UDWCharacterAnim::UDWCharacterAnim()
{
	bFalling = false;
	bSprinting = false;
	bAttacking = false;
	bDefending = false;
	bClimbing = false;
	bCrouching = false;
	bFlying = false;
	bRiding = false;
	bSwimming = false;
	MoveDirection = 0;
	HorizontalSpeed = 0;
	VerticalSpeed = 0;
}

void UDWCharacterAnim::NativeInitializeAnimation()
{
	
}

bool UDWCharacterAnim::HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	const FName notifyName = AnimNotifyEvent.GetNotifyEventName();
	if (notifyName.IsEqual(FName("AnimNotify_Free to animate")))
	{
		OwnerCharacter->FreeToAnim();
	}
	else if (notifyName.IsEqual(FName("AnimNotify_Attack start")))
	{
		OwnerCharacter->AttackStart();
	}
	else if (notifyName.IsEqual(FName("AnimNotify_Attack hurt")))
	{
		OwnerCharacter->AttackHurt();
	}
	else if (notifyName.IsEqual(FName("AnimNotify_Attack end")))
	{
		OwnerCharacter->AttackEnd();
	}
	return false;
}

void UDWCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!OwnerCharacter) OwnerCharacter = Cast<ADWCharacter>(TryGetPawnOwner());

	if (!OwnerCharacter) return;

	UpdateAnimParams(DeltaSeconds);
}

void UDWCharacterAnim::UpdateAnimParams(float DeltaSeconds)
{
	if (!OwnerCharacter) return;

	bFalling = OwnerCharacter->IsFalling();

	bSprinting = OwnerCharacter->IsSprinting();
	bAttacking = OwnerCharacter->IsAttacking();
	bDefending = OwnerCharacter->IsDefending();
	bFlying = OwnerCharacter->IsFlying();
	bRiding = OwnerCharacter->IsRiding();
	bClimbing = OwnerCharacter->IsClimbing();
	bCrouching = OwnerCharacter->IsCrouching();
	bSwimming = OwnerCharacter->IsSwimming();

	VerticalSpeed = OwnerCharacter->GetMoveVelocity(false).Z;
	HorizontalSpeed = OwnerCharacter->GetMoveVelocity().Size();

	MoveDirection = FMath::FindDeltaAngleDegrees(OwnerCharacter->GetMoveDirection().ToOrientationRotator().Yaw, OwnerCharacter->GetActorRotation().Yaw);
}
