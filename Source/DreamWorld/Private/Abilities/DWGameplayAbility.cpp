#include "Abilities/DWGameplayAbility.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "Character/DWCharacter.h"
#include "Abilities/DWTargetType.h"

UDWGameplayAbility::UDWGameplayAbility()
{
	EffectContainerMap = TMap<FGameplayTag, FDWGameplayEffectContainer>();
}

void UDWGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr*/)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

FDWGameplayEffectContainerSpec UDWGameplayAbility::MakeEffectContainerSpecFromContainer(const FDWGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	// 获取数据
	FDWGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	UDWAbilitySystemComponent* OwningASC = UDWAbilitySystemComponent::GetAbilitySystemComponentFormActor(OwningActor);

	if (OwningASC)
	{
		// 如果有目标类型，运行目标逻辑
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UDWTargetType* TargetType = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetType->GetTargets(OwningActor, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}
		// 如果没有复写GameplayLevel就使用自身的
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = GetAbilityLevel();
		}
		// 从GameplayEffect列表创建GameplayEffectSpecs
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FDWGameplayEffectContainerSpec UDWGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	FDWGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);
	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FDWGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UDWGameplayAbility::ApplyEffectContainerSpec(const FDWGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;
	// 将GameplayEffect应用到目标
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UDWGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	FDWGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}
