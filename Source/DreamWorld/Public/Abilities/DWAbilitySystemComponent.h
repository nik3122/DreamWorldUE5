#pragma once

#include "DreamWorld.h"
#include "AbilitySystemComponent.h"
#include "DWAbilitySystemComponent.generated.h"

class UDWGameplayAbility;

/**
 * AbilitySystem组件
 */
UCLASS()
class DREAMWORLD_API UDWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UDWAbilitySystemComponent();

	/** 根据标签获取当前激活的Ability列表 */
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer & AbilityTags, TArray<UDWGameplayAbility*>& ActiveAbilities);

	/** 获取Ability的默认等级，由角色派生 */
	int32 GetDefaultAbilityLevel() const;

public:
	/** 通过Actor获取AbilitySystemComponent */
	static UDWAbilitySystemComponent* GetAbilitySystemComponentFormActor(const AActor* Actor, bool LookForComponent = false);
};