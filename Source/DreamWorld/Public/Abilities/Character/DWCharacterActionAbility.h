#pragma once

#include "DreamWorld.h"
#include "Abilities/Character/DWCharacterAbility.h"
#include "DWCharacterActionAbility.generated.h"

/**
 * ��ɫ��ΪAbility����
 */
UCLASS()
class DREAMWORLD_API UDWCharacterActionAbility : public UDWCharacterAbility
{
	GENERATED_BODY()

public:
	UDWCharacterActionAbility();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ECharacterActionType ActionType;
	
protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};