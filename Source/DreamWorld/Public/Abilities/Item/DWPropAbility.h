#pragma once

#include "DreamWorld.h"
#include "Abilities/Item/DWItemAbility.h"
#include "DWPropAbility.generated.h"

/**
 * 道具Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWPropAbility : public UDWItemAbility
{
	GENERATED_BODY()

public:
	UDWPropAbility();
};