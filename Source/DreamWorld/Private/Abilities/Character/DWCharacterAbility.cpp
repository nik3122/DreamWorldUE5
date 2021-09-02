#include "Abilities/Character/DWCharacterAbility.h"
#include "Character/DWCharacter.h"

UDWCharacterAbility::UDWCharacterAbility()
{
	OwnerCharacter = nullptr;
	AnimMontage = nullptr;
}

void UDWCharacterAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if(IsInstantiated())
	{
		OwnerCharacter = Cast<ADWCharacter>(GetOwningActorFromActorInfo());
	}
}
