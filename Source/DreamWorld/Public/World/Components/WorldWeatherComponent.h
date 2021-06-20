// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Components/ActorComponent.h"
#include "WorldWeatherComponent.generated.h"

class ASkyLight;
class ADirectionalLight;

/**
 * 天气管理组件
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UWorldWeatherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldWeatherComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateWeather();
};