// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DreamWorld.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DWAbilityTask_WaitForEvent.generated.h"

class UDWAbilitySystemComponent;

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDWWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * This task combines Wait and WaitForEvent into one task, so you can wait for multiple types of activations such as from a melee combo
 * Much of this code is copied from one of those two ability tasks
 * This is a good task to look at as an example when creating game-specific tasks
 * It is expected that each game will have a set of game-specific tasks to do what they want
 */
UCLASS()
class DREAMWORLD_API UDWAbilityTask_WaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UDWAbilityTask_WaitForEvent(const FObjectInitializer& ObjectInitializer);
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FDWWaitForEventDelegate EventReceived;

	/**
	 * Play a montage and wait for it end. If a gameplay event happens that matches EventTags (or EventTags is empty), the EventReceived delegate will fire with a tag and event data.
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param MontageToPlay The montage to play on the character
	 * @param EventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param Rate Change to play the montage faster or slower
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UDWAbilityTask_WaitForEvent* WaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FGameplayTagContainer EventTags);

private:
	/** List of tags to match against gameplay events */
	UPROPERTY()
	FGameplayTagContainer EventTags;

	/** Returns our ability system component */
	UDWAbilitySystemComponent* GetTargetASC();

	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FDelegateHandle EventHandle;
};
