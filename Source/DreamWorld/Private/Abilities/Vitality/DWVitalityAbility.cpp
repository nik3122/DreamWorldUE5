#include "Abilities/Vitality/DWVitalityAbility.h"
#include "Vitality/VitalityObject.h"

UDWVitalityAbility::UDWVitalityAbility()
{
	
}

AVitalityObject* UDWVitalityAbility::GetOwnerVitality() const
{
	return Cast<AVitalityObject>(GetOwningActorFromActorInfo());
}
