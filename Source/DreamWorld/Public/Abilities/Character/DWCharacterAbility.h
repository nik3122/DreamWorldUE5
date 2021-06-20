#pragma once

#include "DreamWorld.h"
#include "Abilities/DWGameplayAbility.h"
#include "DWCharacterAbility.generated.h"

class ADWCharacter;

/**
 * 角色Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWCharacterAbility : public UDWGameplayAbility
{
	GENERATED_BODY()

public:
	UDWCharacterAbility();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};