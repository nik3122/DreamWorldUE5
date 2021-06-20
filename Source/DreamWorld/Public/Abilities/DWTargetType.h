#pragma once

#include "DreamWorld.h"
#include "NoExportTypes.h"
#include "GameplayAbilityTypes.h"
#include "DWTargetType.generated.h"
 
 class IVitality;

/**
 * 能力目标基类
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class DREAMWORLD_API UDWTargetType : public UObject
{
	GENERATED_BODY()

public:
	UDWTargetType() {}

	/** 获取目标 */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(AActor* OwningActor, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/**
 * 能力目标_使用拥有者
 */
UCLASS(NotBlueprintable)
class DREAMWORLD_API UDWTargetType_UseOwner : public UDWTargetType
{
	GENERATED_BODY()

public:
	UDWTargetType_UseOwner() {}
	
	virtual void GetTargets_Implementation(AActor* OwningActor, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/**
 * 能力目标_使用事件数据
 */
UCLASS(NotBlueprintable)
class DREAMWORLD_API UDWTargetType_UseEventData : public UDWTargetType
{
	GENERATED_BODY()

public:
	UDWTargetType_UseEventData() {}

	virtual void GetTargets_Implementation(AActor* OwningActor, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
