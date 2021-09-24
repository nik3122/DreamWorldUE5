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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffects")
	TArray<TSubclassOf<UGameplayEffect>> ItemEffectClasses;

private:
	TArray<FActiveGameplayEffectHandle> ItemEffectHandles;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};