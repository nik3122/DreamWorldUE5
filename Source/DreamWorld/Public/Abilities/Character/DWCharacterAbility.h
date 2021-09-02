#pragma once

#include "DreamWorld.h"
#include "Abilities/DWGameplayAbility.h"
#include "DWCharacterAbility.generated.h"

class ADWCharacter;

/**
 * 角色Ability基类
 */
UCLASS()
class DREAMWORLD_API UDWCharacterAbility : public UDWGameplayAbility
{
	GENERATED_BODY()

public:
	UDWCharacterAbility();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ADWCharacter* OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AnimMontage;

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};