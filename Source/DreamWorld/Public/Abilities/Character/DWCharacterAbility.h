#pragma once

#include "DreamWorld.h"
#include "Abilities/DWGameplayAbility.h"
#include "DWCharacterAbility.generated.h"

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAutoEndAbility;

public:
	UFUNCTION(BlueprintPure)
	class ADWCharacter* GetOwnerCharacter() const;
};