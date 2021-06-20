#pragma once

#include "DWTypes.h"
#include "Abilities/GameplayAbility.h"
#include "DWGameplayAbility.generated.h"

/**
 * GameplayAbility基类
 */
UCLASS()
class DREAMWORLD_API UDWGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDWGameplayAbility();

public:
	/** GameplayEffectr容器列表 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffects")
	TMap<FGameplayTag, FDWGameplayEffectContainer> EffectContainerMap;

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;

public:
	/** 从GameplayEffect容器创建GameplayEffect容器规范 */
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual FDWGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FDWGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** 从标签容器创建GameplayEffect容器规范 */
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual FDWGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** 应用GameplayEffect容器规范 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FDWGameplayEffectContainerSpec& ContainerSpec);

	/** 应用GameplayEffect容器 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData,	int32 OverrideGameplayLevel = -1);
};