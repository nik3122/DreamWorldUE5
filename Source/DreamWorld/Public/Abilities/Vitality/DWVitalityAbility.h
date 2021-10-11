#pragma once

#include "DreamWorld.h"
#include "Abilities/DWGameplayAbility.h"
#include "DWVitalityAbility.generated.h"

/**
 * Vitality Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWVitalityAbility : public UDWGameplayAbility
{
	GENERATED_BODY()

public:
	UDWVitalityAbility();

public:
	UFUNCTION(BlueprintPure)
	class AVitalityObject* GetOwnerVitality() const;
};