#include "Abilities/Character/DWCharacterActionAbility.h"

#include "DWCharacter.h"

UDWCharacterActionAbility::UDWCharacterActionAbility()
{
	ActionType = ECharacterActionType::None;
}

void UDWCharacterActionAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if(!bWasCancelled && GetOwnerCharacter())
	{
		GetOwnerCharacter()->StopAction(ActionType, false, true);
	}
}
