#pragma once

#include "DreamWorld.h"
#include "Abilities/DWGameplayAbility.h"
#include "DWItemAbility.generated.h"

/**
 * 物品Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWItemAbility : public UDWGameplayAbility
{
	GENERATED_BODY()

public:
	UDWItemAbility();
};