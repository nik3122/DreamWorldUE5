#pragma once

#include "DreamWorld.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DWDamageExecution.generated.h"

/**
 * 伤害计算及执行类
 */
UCLASS()
class DREAMWORLD_API UDWDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDWDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};