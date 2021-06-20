#pragma once

#include "DreamWorld.h"
#include "Abilities/Item/DWItemAbility.h"
#include "DWEquipAbility.generated.h"

/**
 * 装备Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWEquipAbility : public UDWItemAbility
{
	GENERATED_BODY()

public:
	UDWEquipAbility();
};