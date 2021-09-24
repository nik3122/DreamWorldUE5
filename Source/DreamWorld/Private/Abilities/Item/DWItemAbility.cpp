#include "Abilities/Item/DWItemAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

UDWItemAbility::UDWItemAbility()
{
	
}

void UDWItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	for(auto Iter : ItemEffectClasses)
	{
		const FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Iter, GetCurrentAbilitySpec()->Level);
		ItemEffectHandles.Add(ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpec));
	}
}

void UDWItemAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if(bWasCancelled)
	{
		for(auto Iter : ItemEffectHandles)
		{
			BP_RemoveGameplayEffectFromOwnerWithHandle(Iter);
		}
	}
}
