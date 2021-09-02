#include "Abilities/Item/DWItemAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

UDWItemAbility::UDWItemAbility()
{
	ItemEffectClass = nullptr;
}

void UDWItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(ItemEffectClass, GetCurrentAbilitySpec()->Level);
	ItemEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpec);
}

void UDWItemAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if(bWasCancelled && ItemEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ItemEffectHandle);
	}
}
