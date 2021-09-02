#include "Abilities/DWAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/DWGameplayAbility.h"
#include "AbilitySystemGlobals.h"
#include "Character/DWCharacter.h"
#include "GameplayAbilitySpec.h"

UDWAbilitySystemComponent::UDWAbilitySystemComponent() { }

void UDWAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& AbilityTags, TArray<UDWGameplayAbility*>& ActiveAbilities)
{
	// 获取标签对应的已激活的Ability列表
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTags, AbilitiesToActivate, false);

	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// 获取Ability实例列表
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			// 将Ability实例加入返回列表中
			ActiveAbilities.Add(Cast<UDWGameplayAbility>(ActiveAbility));
		}
	}
}

int32 UDWAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ADWCharacter* OwnerCharacter = Cast<ADWCharacter>(GetOwnerActor());
	if (OwnerCharacter)
	{
		return OwnerCharacter->GetLevelC();
	}
	return 1;
}

UDWAbilitySystemComponent* UDWAbilitySystemComponent::GetAbilitySystemComponentFormActor(const AActor* Actor, bool LookForComponent /*= false*/)
{
	return Cast<UDWAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
