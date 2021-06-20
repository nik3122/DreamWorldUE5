// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Abilities/Tasks/DWAbilityTask_WaitForEvent.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"

UDWAbilityTask_WaitForEvent::UDWAbilityTask_WaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UDWAbilitySystemComponent* UDWAbilityTask_WaitForEvent::GetTargetASC()
{
	return Cast<UDWAbilitySystemComponent>(AbilitySystemComponent);
}

void UDWAbilityTask_WaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UDWAbilityTask_WaitForEvent* UDWAbilityTask_WaitForEvent::WaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, FGameplayTagContainer EventTags)
{
	UDWAbilityTask_WaitForEvent* MyObj = NewAbilityTask<UDWAbilityTask_WaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->EventTags = EventTags;

	return MyObj;
}

void UDWAbilityTask_WaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	UDWAbilitySystemComponent* DWAbilitySystemComponent = GetTargetASC();

	if (DWAbilitySystemComponent)
	{
		EventHandle = DWAbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UDWAbilityTask_WaitForEvent::OnGameplayEvent));
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UDWAbilityTask_WaitForEvent called on invalid AbilitySystemComponent"));
	}

	SetWaitingOnAvatar();
}

void UDWAbilityTask_WaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	Super::ExternalCancel();
}

void UDWAbilityTask_WaitForEvent::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	UDWAbilitySystemComponent* DWAbilitySystemComponent = GetTargetASC();
	if (DWAbilitySystemComponent)
	{
		DWAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}
