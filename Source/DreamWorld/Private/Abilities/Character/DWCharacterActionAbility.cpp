#include "Abilities/Character/DWCharacterActionAbility.h"
#include "Character/DWCharacter.h"

UDWCharacterActionAbility::UDWCharacterActionAbility()
{
	AnimMontage = nullptr;

}

void UDWCharacterActionAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
