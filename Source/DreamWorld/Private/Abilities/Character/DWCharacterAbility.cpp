#include "Abilities/Character/DWCharacterAbility.h"
#include "Character/DWCharacter.h"

UDWCharacterAbility::UDWCharacterAbility()
{
	AnimMontage = nullptr;
	bAutoEndAbility = true;
}

ADWCharacter* UDWCharacterAbility::GetOwnerCharacter() const
{
	return Cast<ADWCharacter>(GetOwningActorFromActorInfo());
}
